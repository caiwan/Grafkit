#include "stdafx.h"

#include "SceneLoader.h"

#include "../utils/asset.h"
#include "../utils/persistence/archive.h"

#include "../utils/ResourceManager.h"

using namespace Grafkit;
using namespace FWdebugExceptions;

Grafkit::SceneLoader::SceneLoader(std::string name, std::string source_name) : IResourceBuilder(name, source_name)
{
}

Grafkit::SceneLoader::~SceneLoader()
{
}

void Grafkit::SceneLoader::Load(Grafkit::IResourceManager * const & resman, Grafkit::IResource * source)
{
	SceneResRef dstScene = dynamic_cast<SceneRes*>(source);
	if (dstScene.Invalid()) {
		throw EX(NullPointerException);
	}

	IAssetRef asset = this->GetSourceAsset(resman);

	ArchiveMemory ar((BYTE*)asset->GetData(), asset->GetSize());

	SceneRef scene;

	SceneLoader::SceneLoaderHelper loader(ar, scene);
	loader.Load(ar, resman);

	dstScene->AssingnRef(scene);
}

void Grafkit::SceneLoader::Save(SceneRes scene, std::string dst_name)
{
	FILE *fp = nullptr;
	fopen_s(&fp, dst_name.c_str(), "wb");
	if (!fp) throw EX_DETAILS(SceneLoadException, "cannot serialize object");

	ArchiveFile ar(fp, true);

	SceneLoader::SceneLoaderHelper loader(ar, scene);
	loader.Save(ar);

	fflush(fp);
	fclose(fp);

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

void Grafkit::SceneLoader::SceneLoaderHelper::Load(Archive &ar, IResourceManager * const & resman)
{
	PERSIST_REFOBJECT(ar, m_scene);

	if (m_scene.Invalid())
		throw EX(SceneLoadException);

	PersistMaterials(ar, resman);
	PersistEntities(ar, resman);
	PersistActors(ar, resman);
	PersistAnimations(ar, resman);

	// 5. material -> mesh relation
	//LOGGER(Log::Logger().Info("Materials:"));
	PersistKeymap(ar, m_materials_to_meshes);
	for (auto key_it = m_materials_to_meshes.begin(); key_it != m_materials_to_meshes.end(); ++key_it)
	{
		USHORT key = key_it->first;
		Material * material = m_materials[key];
		for (auto value_it = key_it->second.begin(); value_it != key_it->second.end(); ++value_it)
		{
			USHORT val = *value_it;
			Model *model = dynamic_cast<Model *>(m_entities[val]);

			if (model) {
				model->SetMaterial(material);

				//LOGGER(Log::Logger().Info("%hu -> %hu", key, val));
			}
		}
	}
	// ... 

	// 6. entitiy -> actor relation
	//LOGGER(Log::Logger().Info("Entities:"));
	PersistKeymap(ar, m_entities_to_actors);
	for (auto key_it = m_entities_to_actors.begin(); key_it != m_entities_to_actors.end(); ++key_it)
	{
		USHORT key = key_it->first;
		Entity3D *entity = m_entities[key];
		for (auto value_it = key_it->second.begin(); value_it != key_it->second.end(); ++value_it)
		{
			USHORT val = *value_it;
			Actor *actor = m_actors[val];
			actor->AddEntity(entity);

			//LOGGER(Log::Logger().Info("%hu -> %hu %s", key, val, entity->GetName().c_str()));
		}
	}

	// ...

	// 7. actor -> actor relation - scenegraph
	//LOGGER(Log::Logger().Info("Actors:"));
	PersistKeymap(ar, m_actor_to_actor);
	for (auto key_it = m_actor_to_actor.begin(); key_it != m_actor_to_actor.end(); ++key_it)
	{
		USHORT key = key_it->first;

		for (auto value_it = key_it->second.begin(); value_it != key_it->second.end(); ++value_it)
		{
			USHORT val = *value_it;
			Actor *child = m_actors[val];
			Actor *parent = m_actors[key];
			if (parent && child)
				parent->AddChild(child);

			//LOGGER(Log::Logger().Info("%hu -> %hu %d", key, val, parent != nullptr));
		}
	}

	Actor * root = m_actors[0];
	m_scene->Initialize(root);
}

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


// ======================================================================================================================
// Store

void Grafkit::SceneLoader::SceneLoaderHelper::Save(Archive &ar)
{
	// --- persist scene 
	PERSIST_REFOBJECT(ar , m_scene);

	IResourceManager * const resman = nullptr;

	PersistMaterials(ar, resman);
	PersistEntities(ar, resman);
	PersistActors(ar, resman);
	PersistAnimations(ar, resman);

	PersistKeymap(ar, m_materials_to_meshes);
	PersistKeymap(ar, m_entities_to_actors);
	PersistKeymap(ar, m_actor_to_actor);
}

void Grafkit::SceneLoader::SceneLoaderHelper::PersistMaterials(Archive &ar, IResourceManager * const & resman)
{
	UINT materialCount = 0;
	if (ar.IsStoring())
		materialCount = m_materials.size();

	PERSIST_FIELD(ar , materialCount);
	for (UINT i = 0; i < materialCount; ++i) {
		
		Material *material = nullptr; 
		if (ar.IsStoring()) 
			material = m_materials[i];

		PERSIST_OBJECT(ar , material);

		if (!ar.IsStoring())
			m_materials.push_back(material);

		// materials_to_textures
		std::vector<texture_bind_t> tx_bind;
		UINT textureCount = 0;

		if (ar.IsStoring()) {
			tx_bind = m_textures_to_materials[i];
			textureCount = tx_bind.size();
		}

		PERSIST_FIELD(ar , textureCount);
		
		for (UINT j = 0; j < textureCount; ++j) {
			texture_bind_t tx;
			if (ar.IsStoring())
				tx = tx_bind[j];

			PERSIST_STRING(ar , tx.bind);
			PERSIST_STRING(ar , tx.name);

			if (!ar.IsStoring()) {
				TextureResRef texture = resman->Get<TextureRes>(tx.name);
				material->AddTexture(texture, tx.bind);
			}
		}
	}
}

void Grafkit::SceneLoader::SceneLoaderHelper::PersistEntities(Archive &ar, IResourceManager * const & resman)
{
	UINT entityCount = 0;
	if (ar.IsStoring())
		entityCount = m_entities.size();
	
	PERSIST_FIELD(ar, entityCount);
	for (UINT i = 0; i < entityCount; ++i) {
		Entity3D* entity = nullptr;
		if (ar.IsStoring())
			entity = m_entities[i];

		PERSIST_OBJECT(ar, entity);
		
		if (!ar.IsStoring()) {
			m_entities.push_back(entity);
		}
	}
}

void Grafkit::SceneLoader::SceneLoaderHelper::PersistActors(Archive &ar, IResourceManager * const & resman)
{
	UINT actorCount = 0;
	if (ar.IsStoring())
		actorCount = m_actors.size();

	PERSIST_FIELD(ar, actorCount);
	for (UINT i = 0; i < actorCount; ++i) {
		Actor* actor = nullptr;
		if (ar.IsStoring())
			Actor* actor = m_actors[i];

		PERSIST_OBJECT(ar , actor);

		if (!ar.IsStoring())
			m_actors.push_back(actor);
	}
}

void Grafkit::SceneLoader::SceneLoaderHelper::PersistAnimations(Archive &ar, IResourceManager * const & resman)
{
	// TODO
}

void Grafkit::SceneLoader::SceneLoaderHelper::PersistKeymap(Archive & ar, SceneLoader::SceneLoaderHelper::assoc_t & keymap)
{
	if (!ar.IsStoring())
		keymap.clear();

	USHORT keyMapCount = keymap.size();
	PERSIST_FIELD(ar, keyMapCount);
	
	auto key_it = keymap.cbegin();
	auto key_end = keymap.cend();
	int loop = 1;

	USHORT i = 0;
	while (loop)
	{
		USHORT key = 0, valuelen = 0;
		
		if (ar.IsStoring())
			key = key_it->first, valuelen = key_it->second.size();

		PERSIST_FIELD(ar, key);
		PERSIST_FIELD(ar, valuelen);

		std::vector<USHORT> values;
		if (ar.IsStoring())
			values = key_it->second;

		for (size_t j = 0; j<valuelen; ++j)
		{
			USHORT value = 0;
			if (ar.IsStoring())
				value = values[j];

			PERSIST_FIELD(ar, value);

			if (!ar.IsStoring())
				values.push_back(value);
		}
		
		if (!ar.IsStoring()) {
			keymap[i] = values;
		}

		if (ar.IsStoring()) {
			if (loop) ++key_it;
			loop = key_it != key_end;
		}
		else {
			loop = i < keyMapCount;
		}

		++i;

	}
}
