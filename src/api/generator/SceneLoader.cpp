#include "stdafx.h"

#include "SceneLoader.h"

#include "../utils/asset.h"
#include "../utils/persistence/archive.h"

#include "../utils/ResourceManager.h"

using namespace Grafkit;
using namespace FWdebugExceptions;

class SceneLoaderHelper {

};


Grafkit::SceneLoader::SceneLoader(std::string name, std::string source_name) : IResourceBuilder(name, source_name)
{
}

Grafkit::SceneLoader::~SceneLoader()
{
}




void Grafkit::SceneLoader::Load(Grafkit::IResourceManager * const & assman, Grafkit::IResource * source)
{
	SceneResRef dstScene = dynamic_cast<SceneRes*>(source);
	if (dstScene.Invalid()) {
		throw EX(NullPointerException);
	}

	IAssetRef asset = this->GetSourceAsset(assman);

	ArchiveMemory ar((BYTE*)asset->GetData(), asset->GetSize());

	SceneRef scene;
	PERSIST_REFOBJECT(ar, scene);

	if (scene.Invalid())
		throw EX(SceneLoadException);
	
	SceneLoader::SceneLoaderHelper loader(ar, scene);
	loader.Load();

	// --- 
	std::vector<MaterialRef> materials;
	texture_assoc_t textures_to_materials;
	std::vector<Ref<Entity3D>> entities;
	std::vector<ActorRef> actors;
	std::vector<AnimationRef> animations;

	// 1. materials
	UINT materialCount = materials.size();
	PERSIST_FIELD(ar, materialCount);
	for (UINT i = 0; i < materialCount; ++i) {
		Material *material = nullptr;
		PERSIST_OBJECT(ar, material);
		materials.push_back(material);

		// materials to textures
		UINT textureCount = 0;
		PERSIST_FIELD(ar, textureCount);
		for (UINT j = 0; j < textureCount; ++j) {
			texture_bind_t tx;
			PERSIST_STRING(ar, tx.bind);
			PERSIST_STRING(ar, tx.name);

			TextureResRef texture = assman->Get<TextureRes>(tx.name);
			material->AddTexture(texture, tx.bind);
			//LOGGER(Log::Logger().Info("Texture: %s" , tx.name.c_str()));
		}
	}

	// 2. entities
	UINT entityCount = entities.size();
	PERSIST_FIELD(ar, entityCount);
	for (UINT i = 0; i < entityCount; ++i) {
		Entity3D* entity = nullptr;
		PERSIST_OBJECT(ar, entity);
		entities.push_back(entity);
	}

	// 3. actors
	UINT actorCount = actors.size();
	PERSIST_FIELD(ar, actorCount);
	for (UINT i = 0; i < actorCount; ++i) {
		Actor* actor = nullptr;
		PERSIST_OBJECT(ar, actor);
		actors.push_back(actor);
	}

	// 4. animations
	// TODO

	assoc_t materials_to_meshes;
	assoc_t entities_to_actors;
	assoc_t actor_to_actor;

	// 5. material -> mesh relation
	//LOGGER(Log::Logger().Info("Materials:"));
	LoadKeymap(ar, materials_to_meshes);
	for (auto key_it = materials_to_meshes.begin(); key_it != materials_to_meshes.end(); ++key_it) 
	{
		USHORT key = key_it->first;
		Material * material = materials[key];
		for (auto value_it = key_it->second.begin(); value_it != key_it->second.end(); ++value_it) 
		{
			USHORT val = *value_it;
			Model *model = dynamic_cast<Model *>(entities[val].Get());
			if (model) {
				model->SetMaterial(material);

				//LOGGER(Log::Logger().Info("%hu -> %hu", key, val));
			}
		}
	}
	// ... 

	// 6. entitiy -> actor relation
	//LOGGER(Log::Logger().Info("Entities:"));
	LoadKeymap(ar, entities_to_actors);
	for (auto key_it = entities_to_actors.begin(); key_it != entities_to_actors.end(); ++key_it) 
	{
		USHORT key = key_it->first;
		Entity3D *entity = entities[key];
		for (auto value_it = key_it->second.begin(); value_it != key_it->second.end(); ++value_it) 
		{
			USHORT val = *value_it;
			Actor *actor = actors[val];
			actor->AddEntity(entity);

			//LOGGER(Log::Logger().Info("%hu -> %hu %s", key, val, entity->GetName().c_str()));
		}
	}

	// ...

	// 7. actor -> actor relation - scenegraph
	//LOGGER(Log::Logger().Info("Actors:"));
	LoadKeymap(ar, actor_to_actor);
	for (auto key_it = actor_to_actor.begin(); key_it != actor_to_actor.end(); ++key_it) 
	{
		USHORT key = key_it->first;
		
		for (auto value_it = key_it->second.begin(); value_it != key_it->second.end(); ++value_it) 
		{
			USHORT val = *value_it;
			Actor *child = actors[val];
			Actor *parent = actors[key];
			if (parent && child)
				parent->AddChild(child);
			
			//LOGGER(Log::Logger().Info("%hu -> %hu %d", key, val, parent != nullptr));
		}
	}

	Actor * root = actors[0];
	scene->Initialize(root);

	dstScene->AssingnRef(scene);
}

void Grafkit::SceneLoader::Save(SceneRes scene, std::string dst_name)
{
	FILE *fp = nullptr;
	fopen_s(&fp, dst_name.c_str(), "wb");
	if (!fp) throw EX_DETAILS(SceneLoadException, "cannot serialize object");

	ArchiveFile ar(fp, true);


	fflush(fp);
	fclose(fp);

}

void LoadKeymap(Archive &ar, assoc_t &keymap){
	keymap.clear();
	USHORT keyMapCount = 0;
	PERSIST_FIELD(ar, keyMapCount);
	for (size_t i=0; i<keyMapCount; ++i)
	{
		USHORT key = 0, valuelen = 0;
		PERSIST_FIELD(ar, key);
		PERSIST_FIELD(ar, valuelen);
		std::vector<USHORT> &values = keymap[i];
		for (size_t j = 0; j<valuelen; ++j)
		{
			USHORT value = 0;
			PERSIST_FIELD(ar, value);
			values.push_back(value);
		}
	}
}

/************************************************************************************************************************
* Scene persist helper
*************************************************************************************************************************/

Grafkit::SceneLoader::SceneLoaderHelper::SceneLoaderHelper(Archive &ar, SceneRef & scene) : m_scene(scene)
{
	if (ar.IsStoring()) {
		BuildObjectMaps();
	}
}

Grafkit::SceneLoader::SceneLoaderHelper::~SceneLoaderHelper()
{
}


// ======================================================================================================================
// Load

// ======================================================================================================================
// Store

// Builds map of object relations
void Grafkit::SceneLoader::SceneLoaderHelper::BuildObjectMaps()
{
	m_cTexID = 0;
	m_cMatID = 0;
	m_cEntityID = 0;
	m_cActorID = 0;

	// collect data
	ActorRef scenegraph = m_scene->GetRootNode();

	std::stack<Actor*> stack;
	stack.push(scenegraph);

	// --- collect assoc. map and tree for serialization 
	int i = 0, j = 0, k = 0, l = 0;
	while (!stack.empty()) {
		ActorRef node = stack.top(); stack.pop();

		if (node.Valid()) {
			m_actors.push_back(node);
			m_actor_map[node] = m_cActorID;

			BuildEntityMap(node);

			++m_cActorID;
		}

		for (auto it = node->GetChildren().begin(); it != node->GetChildren().end(); it++)
			stack.push(*it);
	}

	BuildActorMap();
}

// Which texture belong to this exact material, and which shader slot bound to
void Grafkit::SceneLoader::SceneLoaderHelper::BuildTextureMap(const MaterialRef & material)
{
	auto tx_begin = material->_GetTextureMapIterator_Begin();
	auto tx_end = material->_GetTextureMapIterator_End();

	if (tx_begin != tx_end) {
		for (auto tx_it = tx_begin; tx_it != tx_end; ++tx_it) {

			TextureResRef texture = tx_it->second;
			if (texture.Valid() && texture->Valid()) {
				texture_bind_t bind;
				bind.name = texture->GetName();
				bind.bind = tx_it->first;
				m_textures_to_materials[m_cMatID].push_back(bind);
			
				// .. ami kell meg ide johet

				++m_cTexID;
			}
		}
	}
}

// Which material belongs to which entity if its a model
void Grafkit::SceneLoader::SceneLoaderHelper::BuildMaterialMap(const ModelRef &model)
{
	MaterialRef material = model->GetMaterial();
	if (material.Valid() && m_material_set.find(material) == m_material_set.end()) {
		m_material_set.insert(material);
		m_materials_to_meshes[m_cMatID].push_back(m_cEntityID);
		m_materials.push_back(material);

		BuildTextureMap(material);

		++m_cMatID;
	} // has material
}

// which entity belongs to which actor
void Grafkit::SceneLoader::SceneLoaderHelper::BuildEntityMap(const ActorRef &actor)
{
	for (auto entity_it = actor->GetEntities().begin(); entity_it != actor->GetEntities().end(); ++entity_it) {
		Ref<Entity3D> entity = (*entity_it).Get();
		if (entity.Valid() && m_entity_map.find(entity) == m_entity_map.end()) {
			m_entity_map[entity] = m_cEntityID;
			m_entities.push_back(entity);
			m_entities_to_actors[m_cEntityID].push_back(m_cActorID);

			// model 
			const ModelRef model = dynamic_cast<Model*>((*entity_it).Get());
			if (model.Valid()) {
				BuildMaterialMap(model);
				// ... ide jon, ami kell meg
			} 

			// ... ide jon, ami kell meg 

			++m_cEntityID;
		}
		else {
			m_entities_to_actors[m_entity_map[entity]].push_back(m_cActorID);
		}
	} 
}

// which actor whose child of
void Grafkit::SceneLoader::SceneLoaderHelper::BuildActorMap()
{
	for (size_t i = 0; i<m_actors.size(); ++i)
	{
		Actor *actor = m_actors[i];
		if (actor) {
			Actor *parent = actor->GetParent();
			if (parent) {
				auto it = m_actor_map.find(parent);
				if (it != m_actor_map.end()) {
					UINT key = (it->second), val = i;
					m_actor_to_actor[key].push_back(val);
					//LOGGER(Log::Logger().Info("%hu -> %hu", key, val));
				}
			}
		}
	}
}



void Grafkit::SceneLoader::SceneLoaderHelper::Store(Archive &ar)
{
	// --- persist scene 
	PERSIST_REFOBJECT(ar , m_scene);

	StoreMaterials(ar);
	StoreEntities(ar);
	StoreActors(ar);
	StoreAnimations(ar);

	StoreKeymap(ar, m_materials_to_meshes);
	StoreKeymap(ar, m_entities_to_actors);
	StoreKeymap(ar, m_actor_to_actor);
}

void Grafkit::SceneLoader::SceneLoaderHelper::StoreMaterials(Archive &ar)
{
	UINT materialCount = m_materials.size();
	PERSIST_FIELD(ar , materialCount);
	for (UINT i = 0; i < materialCount; ++i) {
		Material *material = m_materials[i];
		PERSIST_OBJECT(ar , material);

		// materials_to_textures
		std::vector<texture_bind_t> bind = m_textures_to_materials[i];
		UINT textureCount = bind.size();
		PERSIST_FIELD(ar , textureCount);
		for (UINT j = 0; j < textureCount; ++j) {
			PERSIST_STRING(ar , bind[j].bind);
			PERSIST_STRING(ar , bind[j].name);
		}
	}
}

void Grafkit::SceneLoader::SceneLoaderHelper::StoreEntities(Archive &ar)
{
	UINT entityCount = m_entities.size();
	PERSIST_FIELD(ar, entityCount);
	for (UINT i = 0; i < entityCount; ++i) {
		Entity3D* entity = m_entities[i];
		//LOGGER(Log::Logger().Info("Entity: %s", entity->GetName().c_str()));
		PERSIST_OBJECT(ar, entity);
	}
}

void Grafkit::SceneLoader::SceneLoaderHelper::StoreActors(Archive &ar)
{
	UINT actorCount = m_actors.size();
	PERSIST_FIELD(ar, actorCount);
	for (UINT i = 0; i < actorCount; ++i) {
		Actor* actor = m_actors[i];
		PERSIST_OBJECT(ar , actor);
	}
}

void Grafkit::SceneLoader::SceneLoaderHelper::StoreAnimations(Archive &ar)
{
	// TODO
}

void Grafkit::SceneLoader::SceneLoaderHelper::StoreKeymap(Archive & ar, SceneLoader::SceneLoaderHelper::assoc_t & keymap)
{
	USHORT keyMapCount = keymap.size();
	PERSIST_FIELD(ar, keyMapCount);
	for (auto key_it = keymap.begin(); key_it != keymap.end(); ++key_it) {
		USHORT key = key_it->first, valuelen = key_it->second.size();
		PERSIST_FIELD(ar, key);
		PERSIST_FIELD(ar, valuelen);
		std::vector<USHORT> &values = key_it->second;
		for (size_t j = 0; j<valuelen; ++j)
		{
			USHORT value = values[j];
			PERSIST_FIELD(ar, value);
		}
	}
}
