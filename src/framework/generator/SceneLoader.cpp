#include "stdafx.h"

#include "SceneLoader.h"

#include "render/model.h"
#include "render/texture.h"
#include "render/Material.h"
#include "animation/scene.h"
#include "animation/animation.h"

#include "utils/persistence/archive.h"

#include "utils/ResourceManager.h"

using namespace Grafkit;
using namespace FWdebugExceptions;

SceneLoader::SceneLoader(std::string name, std::string source_name) : IResourceBuilder(name, source_name)
{
}

SceneLoader::~SceneLoader()
{
}

void SceneLoader::Load(IResourceManager * const & resman, IResource * source)
{
#if 0
	SceneGraphRef dstScene = dynamic_cast<SceneRes*>(source);
	if (dstScene.Invalid()) {
		throw new EX(NullPointerException);
	}

	IAssetRef asset = this->GetSourceAsset(resman);

	ArchiveMemory ar((BYTE*)asset->GetData(), asset->GetSize());

	SceneGraphRef scene;

	LOGGER(Log::Logger().Info("-- LOAD --"));
	SceneLoader::SceneLoaderHelper loader(ar, scene);
	loader.Load(ar, resman);

	dstScene->AssingnRef(scene);
#endif
}

void SceneLoader::Save(SceneRes scene, std::string dst_name)
{
	FILE *fp = nullptr;
	fopen_s(&fp, dst_name.c_str(), "wb");
	if (!fp) throw new EX_DETAILS(SceneLoadException, "cannot serialize object");

	ArchiveFile ar(fp, true);

	LOGGER(Log::Logger().Info("-- STORE --"));

#if 0
	SceneLoader::SceneLoaderHelper loader(ar, scene);
	loader.Save(ar);
#endif

	fflush(fp);
	fclose(fp);

}

/************************************************************************************************************************
* Scene persist helper
*************************************************************************************************************************/

SceneLoader::SceneLoaderHelper::SceneLoaderHelper(Archive &ar, SceneGraphRef & scene) : m_scene(scene)
{
	if (ar.IsStoring()) {
		BuildObjectMaps();
	}
}

SceneLoader::SceneLoaderHelper::~SceneLoaderHelper()
{
}


// ======================================================================================================================
// Load

void SceneLoader::SceneLoaderHelper::Load(Archive &ar, IResourceManager * const & resman)
{
	Persist(ar, resman);

	LOGGER(Log::Logger().Info("-- ASSIGN --"));

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

			LOGGER(Log::Logger().Info("%hu (\"%s\") -> %hu (\"%s\")", key, entity->GetName().c_str(), val, actor->GetName().c_str()));
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

#if 0
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

#endif


#if 0

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
#endif

	if (m_actors.empty())
		throw new EX_DETAILS(SceneLoadException, "Actors are empty for some reason");

	Actor * root = m_actors[0];

#if 0
	m_scene->Initialize(root);
#endif

}

// ======================================================================================================================
// Association Map building
// ======================================================================================================================

// Builds map of object relations
void SceneLoader::SceneLoaderHelper::BuildObjectMaps()
{
	m_cTexID = 0;
	m_cMatID = 0;
	m_cEntityID = 0;
	m_cActorID = 0;


#if 0

	// collect data
	ActorRef scenegraph = m_scene->GetRootNode();

	std::stack<Actor*> stack;
	stack.push(scenegraph);

	LOGGER(Log::Logger().Info(" --- ENTITIY RELATIONS --- "));

	// --- collect assoc. map and tree for serialization 
	int i = 0, j = 0, k = 0, l = 0;
	while (!stack.empty()) {
		ActorRef node = stack.top(); stack.pop();

		if (node.Valid()) {
			m_actors.push_back(node);
			m_actor_map[node] = m_cActorID;

			// yield
			LOGGER(Log::Logger().Info("Actor: %s %d {", node->GetName().c_str(), m_cActorID));

			BuildEntityMap(node);

			LOGGER(Log::Logger().Info("}"));

			++m_cActorID;
			// /yield

			// push next
			k = node->GetChildrenCount();
			for (int i = 0; i < k; i++)
				stack.push(node->GetChild(i));
		}
	}

	BuildActorMap();
	BuildAnimationMap();
#endif
}

// Which texture belong to this exact material, and which shader slot bound to
void SceneLoader::SceneLoaderHelper::BuildTextureMap(const MaterialRef & material)
{
	std::map<std::string, TextureResRef> textureMap;

	if (!material->GetTextureMap(textureMap))
		return;

	auto tx_begin = textureMap.cbegin();
	auto tx_end = textureMap.cend();

	if (tx_begin != tx_end) {
		for (auto tx_it = tx_begin; tx_it != tx_end; ++tx_it) {

			TextureResRef texture = tx_it->second;

			LOGGER(Log::Logger().Info("      Texture: %s {", texture->GetName().c_str()));

			// the texture itself does not need to be valid
			// we just want to have its name
			if (texture.Valid()) {
				texture_bind_t bind;
				bind.first = texture->GetName();
				bind.second = tx_it->first;
				m_textures_to_materials[m_cMatID].push_back(bind);

				// .. ami kell meg ide johet

				++m_cTexID;
			}

			LOGGER(Log::Logger().Info("      }"));
		}
	}
}

// Which material belongs to which entity if its a model
void SceneLoader::SceneLoaderHelper::BuildMaterialMap(const ModelRef &model)
{
	MaterialRef material = model->GetMaterial();
	USHORT matid = 0;

	if (material.Valid()) {
		auto matit = m_material_map.find(material);
		if (matit == m_material_map.end())
		{

			LOGGER(Log::Logger().Info("    Material: %s %d{", material->GetName().c_str(), m_cMatID));

			m_material_map[material] = m_cMatID;
			m_materials.push_back(material);

			matid = m_cMatID;

			BuildTextureMap(material);

			++m_cMatID;

			LOGGER(Log::Logger().Info("    }"));
		} // has material
		else {
			matid = matit->second;
			LOGGER(Log::Logger().Info("    Material: %s %d", material->GetName().c_str(), matid));
		}

		m_materials_to_meshes.push_back(assoc_t(matid, m_cEntityID));
	}

}

// which animation affects which entity or actor
void SceneLoader::SceneLoaderHelper::BuildAnimationMap()
{


#if 0

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
#endif
}

// which entity belongs to which actor
void SceneLoader::SceneLoaderHelper::BuildEntityMap(const ActorRef &actor)
{
	int k = actor->GetEntityCount();
	for (int i = 0; i < k; i++)
	{
		Entity3D *entity = actor->GetEntity(i);

		LOGGER(Log::Logger().Info("  Entity: %s %d {", entity->GetName().c_str(), m_cEntityID));

		if (m_entity_map.find(entity) == m_entity_map.end()) {
			m_entity_map[entity] = m_cEntityID;
			m_entities.push_back(entity);
			m_entities_to_actors.push_back(assoc_t(m_cEntityID, m_cActorID)); // [m_cEntityID].push_back(m_cActorID);

			// model 
			const ModelRef model = dynamic_cast<Model*>(entity);
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

		LOGGER(Log::Logger().Info("  }"));

	}

}

// which actor whose child of
void SceneLoader::SceneLoaderHelper::BuildActorMap()
{
	for (size_t i = 0; i < m_actors.size(); ++i)
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

void SceneLoader::SceneLoaderHelper::Save(Archive &ar)
{
	IResourceManager * const resman = nullptr;
	Persist(ar, resman);
}

void SceneLoader::SceneLoaderHelper::Persist(Archive & ar, IResourceManager * const & resman)
{

#if 0

	try {
		// --- persist scene 
		PERSIST_REFOBJECT(ar, m_scene);

		if (!ar.IsStoring() && m_scene.Invalid())
			throw new EX(SceneLoadException);

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
	catch (int i) {
		return;
	}
#endif
}

void SceneLoader::SceneLoaderHelper::PersistMaterials(Archive &ar, IResourceManager * const & resman)
{
	UINT materialCount = 0;
	if (ar.IsStoring())
		materialCount = m_materials.size();

	PERSIST_FIELD(ar, materialCount);
	for (UINT i = 0; i < materialCount; ++i) {

		Material *material = nullptr;
		if (ar.IsStoring())
			material = m_materials[i];

		PERSIST_OBJECT(ar, material);

		if (!ar.IsStoring())
			m_materials.push_back(material);

		// materials_to_textures
		std::vector<texture_bind_t> tx_bind;
		UINT textureCount = 0;

		if (ar.IsStoring()) {
			tx_bind = m_textures_to_materials[i];
			textureCount = tx_bind.size();
		}

		PERSIST_FIELD(ar, textureCount);

		for (UINT j = 0; j < textureCount; ++j) {
			texture_bind_t tx;
			if (ar.IsStoring())
				tx = tx_bind[j];

			PERSIST_STRING(ar, tx.first);
			PERSIST_STRING(ar, tx.second);

			if (!ar.IsStoring()) {
				TextureResRef texture = resman->Get<Texture2DRes>(tx.first);
				material->AddTexture(texture, tx.second);
			}
		}
	}
}

void SceneLoader::SceneLoaderHelper::PersistEntities(Archive &ar, IResourceManager * const & resman)
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

void SceneLoader::SceneLoaderHelper::PersistActors(Archive &ar, IResourceManager * const & resman)
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

void SceneLoader::SceneLoaderHelper::PersistAnimations(Archive &ar, IResourceManager * const & resman)
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

void SceneLoader::SceneLoaderHelper::PersistKeymap(Archive & ar, std::vector<assoc_t>& keymap)
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