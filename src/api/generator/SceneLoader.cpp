#include "stdafx.h"

#include "SceneLoader.h"

#include "render/animation.h"
#include "render/Scene.h"
#include "render/model.h"
#include "render/texture.h"
#include "render/Material.h"

#include "utils/asset.h"
#include "utils/persistence/archive.h"

#include "utils/ResourceManager.h"

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

	LOGGER(Log::Logger().Info("-- LOAD --"));
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

	LOGGER(Log::Logger().Info("-- STORE --"));
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
	Persist(ar, resman);

	LOGGER(Log::Logger().Info("-ASSIGN-"));

	// 5. material -> mesh relation
	LOGGER(Log::Logger().Info("Materials: %d", m_materials_to_meshes.size()));
	for (auto it = m_materials_to_meshes.begin(); it != m_materials_to_meshes.end(); ++it)
	{
		USHORT key = it->first;
		USHORT val = it->second;

		Material * material = m_materials[key];	
		Model *model = dynamic_cast<Model *>(m_entities[val]);

		if (model && material) {
			model->SetMaterial(material);

			LOGGER(Log::Logger().Info("%hu (\"%s\") -> %hu (\"%s\")", key, material->GetName().c_str(), val, model->GetName().c_str()));
		}
	}
	// ... 

	// 6. entitiy -> actor relation
	LOGGER(Log::Logger().Info("Entities: %d", m_entities_to_actors.size()));
	for (auto it = m_entities_to_actors.begin(); it != m_entities_to_actors.end(); ++it)
	{
		USHORT key = it->first;
		USHORT val = it->second;
		
		Entity3D *entity = m_entities[key];
		Actor *actor = m_actors[val];

		if (actor && entity) {
			actor->AddEntity(entity);

			LOGGER(Log::Logger().Info("%hu (\"%s\") -> %hu (\"%s\")", key, entity->GetName().c_str()), val, actor->GetName().c_str());
		}
	}

	// ...

	// 7. actor -> actor relation - scenegraph
	LOGGER(Log::Logger().Info("Actors: %d", m_actor_to_actor.size()));
	for (auto it = m_actor_to_actor.begin(); it != m_actor_to_actor.end(); ++it)
	{
		USHORT key = it->first;		
		USHORT val = it->second;

		Actor *child = m_actors[val];
		Actor *parent = m_actors[key];

		if (parent && child) {
			parent->AddChild(child);

			LOGGER(Log::Logger().Info("%hu (\"%s\") -> %hu (\"%s\")", key, parent->GetName().c_str(), val, child->GetName().c_str()));
		}
	}

	 //8, 9 animation -> actor, entity
	LOGGER(Log::Logger().Info("Animations: %d", m_animation_to_actor.size() + m_animation_to_entity.size()));
	for (auto it = m_animation_to_actor.cbegin(); it != m_animation_to_actor.cend(); ++it)
	{
		USHORT key = it->first;
		USHORT value = it->second;
		ActorAnimation *actor_animation = dynamic_cast<ActorAnimation *>(m_Animations[key]);
		if (actor_animation) {
			Actor* actor = m_actors[value];
			if (actor) {
				actor_animation->SetActor(ActorRef(actor));
				m_scene->AddAnimation(actor_animation);
				LOGGER(Log::Logger().Info("Actor %hu (\"%s\") -> %hu (\"%s\")", key, actor->GetName().c_str(), value, actor_animation->GetName().c_str()));
			}
		}

	}

	for (auto it = m_animation_to_entity.cbegin(); it != m_animation_to_entity.cend(); ++it)
	{
		USHORT key = it->first;
		USHORT value = it->second;

		EntityAnimation *entity_animation = dynamic_cast<EntityAnimation*>(m_Animations[key]);
		if (entity_animation) {
			Entity3D* entity = m_entities[value];
			if (entity) {
				entity_animation->SetEntity(Ref<Entity3D>(entity));
				m_scene->AddAnimation(entity_animation);
				LOGGER(Log::Logger().Info("Entity %hu -> %hu", key, value));
			}
		}
		
		
	}

	if (m_actors.empty())
		throw new EX_DETAILS(SceneLoadException, "Actors are empty for some reason");

	Actor * root = m_actors[0];
	m_scene->Initialize(root);
}

// ======================================================================================================================
// Association Map building
// ======================================================================================================================

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
	BuildAnimationMap();
}

// Which texture belong to this exact material, and which shader slot bound to
void Grafkit::SceneLoader::SceneLoaderHelper::BuildTextureMap(const MaterialRef & material)
{
	if (material->_IsTextureMapEmpty())
		return;

	auto tx_begin = material->_GetTextureMapIterator_Begin();
	auto tx_end = material->_GetTextureMapIterator_End();

	if (tx_begin != tx_end) {
		for (auto tx_it = tx_begin; tx_it != tx_end; ++tx_it) {

			TextureResRef texture = tx_it->second;
			if (texture.Valid() && texture->Valid()) {
				texture_bind_t bind;
				bind.first = texture->GetName();
				bind.second = tx_it->first;
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
		m_materials_to_meshes.push_back(assoc_t(m_cMatID, m_cEntityID));
		m_materials.push_back(material);

		BuildTextureMap(material);

		++m_cMatID;
	} // has material
}

// which animation affects which entity or actor
void Grafkit::SceneLoader::SceneLoaderHelper::BuildAnimationMap()
{
	std::vector<AnimationRef> animations;
	m_scene->GetAnimations(animations);

	UINT i = 0;

	for (auto it = animations.cbegin(); it != animations.cend(); ++it, ++i) {
		Animation *animation = dynamic_cast<ActorAnimation*>(it->Get());
		if (animation) {
			//qnd rtty
			ActorAnimation *actor_animation = dynamic_cast<ActorAnimation*>(animation); 
			EntityAnimation *entity_animation = dynamic_cast<EntityAnimation*>(animation);
			
			if (actor_animation) {
				auto actor_it = m_actor_map.find(actor_animation->GetActor());
				if (actor_it != m_actor_map.end()) {
					m_animation_to_actor.push_back(assoc_t(i, actor_it->second)); //[i].push_back(actor_it->second);
				}
			}
			
			else if (entity_animation) {
				auto entity_it = m_entity_map.find(entity_animation->GetEntity());
				if (entity_it != m_entity_map.end()) {
					m_animation_to_entity.push_back(assoc_t(i, entity_it->second)); //[i].push_back(entity_it->second);
				}
			}
			
			m_Animations.push_back(it->Get());
		}
	}
}

// which entity belongs to which actor
void Grafkit::SceneLoader::SceneLoaderHelper::BuildEntityMap(const ActorRef &actor)
{
	for (auto entity_it = actor->GetEntities().begin(); entity_it != actor->GetEntities().end(); ++entity_it) {
		if (entity_it->Valid()) {
			Entity3D *entity = (*entity_it).Get();
			if (m_entity_map.find(entity) == m_entity_map.end()) {
				m_entity_map[entity] = m_cEntityID;
				m_entities.push_back(entity);
				m_entities_to_actors.push_back(assoc_t(m_cEntityID, m_cActorID)); // [m_cEntityID].push_back(m_cActorID);

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
				m_entities_to_actors.push_back(assoc_t(m_entity_map[entity], m_cActorID));
			}
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
					m_actor_to_actor.push_back(assoc_t(key, val)); 
				}
			}
		}
	}
}


// ======================================================================================================================
// Persist 
// ======================================================================================================================

void Grafkit::SceneLoader::SceneLoaderHelper::Save(Archive &ar)
{
	IResourceManager * const resman = nullptr;
	Persist(ar, resman);
}

void Grafkit::SceneLoader::SceneLoaderHelper::Persist(Archive & ar, IResourceManager * const & resman)
{
	// --- persist scene 
	PERSIST_REFOBJECT(ar, m_scene);

	if (!ar.IsStoring() && m_scene.Invalid())
		throw EX(SceneLoadException);

	PersistMaterials(ar, resman);
	PersistEntities(ar, resman);
	PersistActors(ar, resman);
	PersistAnimations(ar, resman);

	PersistKeymap(ar, m_materials_to_meshes);
	PersistKeymap(ar, m_entities_to_actors);
	PersistKeymap(ar, m_actor_to_actor);
	PersistKeymap(ar, m_animation_to_actor);
	PersistKeymap(ar, m_animation_to_entity);
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

			PERSIST_STRING(ar, tx.first);
			PERSIST_STRING(ar, tx.second);

			if (!ar.IsStoring()) {
				TextureResRef texture = resman->Get<TextureRes>(tx.first);
				material->AddTexture(texture, tx.second);
			}
		}
	}
}

void Grafkit::SceneLoader::SceneLoaderHelper::PersistTextures(Archive & ar, MaterialRef & material, IResourceManager * const & resman)
{
	// ... 
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
			actor = m_actors[i];

		PERSIST_OBJECT(ar, actor);

		if (!ar.IsStoring())
			m_actors.push_back(actor);
	}
}

void Grafkit::SceneLoader::SceneLoaderHelper::PersistAnimations(Archive &ar, IResourceManager * const & resman)
{
	UINT animationCount = 0;
	if (ar.IsStoring())
		animationCount = m_Animations.size();

	PERSIST_FIELD(ar, animationCount);

	for (UINT i = 0; i < animationCount; ++i) {
		Animation* animation = nullptr;

		if (ar.IsStoring())
			animation = m_Animations[i];

		PERSIST_OBJECT(ar, animation);

		if (!ar.IsStoring())
			m_Animations.push_back(animation);
	}
}

void Grafkit::SceneLoader::SceneLoaderHelper::PersistKeymap(Archive & ar, std::vector<assoc_t>& keymap)
{
	UINT size = 0;
	assoc_t pair;

	if (ar.IsStoring())
		size = keymap.size();
	else
		keymap.clear();

	PERSIST_FIELD(ar, size);

	for (size_t i = 0; i < size; ++i) {
	
		if (ar.IsStoring())
			pair = keymap[i];

		PERSIST_FIELD(ar, pair.first);
		PERSIST_FIELD(ar, pair.second);

		if (!ar.IsStoring())
			keymap.push_back(pair);
	}
}