#include "stdafx.h"

#include "SceneLoader.h"

#include "../utils/asset.h"
#include "../utils/persistence/archive.h"

using namespace Grafkit;
using namespace FWdebugExceptions;

Grafkit::SceneLoader::SceneLoader(std::string name, std::string source_name) : IResourceBuilder(name, source_name)
{
}

Grafkit::SceneLoader::~SceneLoader()
{
}

// assoc. tabla
typedef  std::map<USHORT, std::vector<USHORT>> assoc_t;

void LoadKeymap(Archive &ar, assoc_t &keymap);
void StoreKeymap(Archive &ar, assoc_t &keymap);

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
	
	// --- 
	std::vector<MaterialRef> materials;
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

	PERSIST_REFOBJECT(ar, scene);

	// collect data
	ActorRef scenegraph = scene->GetRootNode();

	std::vector<Actor*> actors;
	std::vector<Entity3D*> entities;
	std::vector<Material*> materials;

	std::set<Entity3D*> entity_set;
	std::set<Material*> material_set;
	std::map<Actor*, int> actor_map;

	assoc_t materials_to_meshes;
	assoc_t entities_to_actors;
	assoc_t actor_to_actor;

	std::stack<Actor*> stack;
	stack.push(scenegraph);

	// --- collect assoc. map and tree for serialization 
	int i = 0, j = 0, k = 0, l = 0;
	while (!stack.empty()) {
		ActorRef node = stack.top(); stack.pop();
		
		// <yield>
		if (node) {
			ActorRef actor = node.Get();
			actors.push_back(actor); 
			actor_map[actor] = i;

			for (auto entity_it = actor->GetEntities().begin(); entity_it != actor->GetEntities().end(); ++entity_it) {
				Ref<Entity3D> entity = (*entity_it).Get();
				if (entity.Valid() && entity_set.find(entity) == entity_set.end()) {
					entity_set.insert(entity);
					entities_to_actors[j].push_back(i);
					entities.push_back(entity);

					const Model * model = dynamic_cast<Model*>((*entity_it).Get());
					if (model) {
						MaterialRef material = model->GetMaterial();
						if (material.Valid() && material_set.find(material) == material_set.end()) {
							material_set.insert(material);
							materials_to_meshes[k].push_back(j);
							materials.push_back(material);
							++k;
						} // has material
					} // is model
					++j; // entity count
				} // is entity
			} // fetch entity
			++i; // actor count
		}
		// </yield>
		
		for (auto it = node->GetChildren().begin(); it != node->GetChildren().end(); it++)
			stack.push(*it);
	}

	//for (auto actor_it = actorsbegin(); actor_it != actors.end(); ++actor_it) {
	for (size_t i = 0; i<actors.size(); ++i)
	{
		Actor *actor = actors[i];
		if (actor) {
			Actor *parent = actor->GetParent();
			if (parent) {
				auto it = actor_map.find(parent);
				if (it != actor_map.end()) {
					UINT key = (it->second), val = i;
					actor_to_actor[key].push_back(val);
					//LOGGER(Log::Logger().Info("%hu -> %hu", key, val));
				}
			}
		}
	}
	
	// --- persist scene 

	// 1. materials
	UINT materialCount = materials.size();
	PERSIST_FIELD(ar, materialCount);
	for (UINT i = 0; i < materialCount; ++i) {
		Material *material = materials[i];
		PERSIST_OBJECT(ar, material);
	}

	// 2. entity
	UINT entityCount = entities.size();
	PERSIST_FIELD(ar, entityCount);
	for (UINT i = 0; i < entityCount; ++i) {
		Entity3D* entity = entities[i];
		//LOGGER(Log::Logger().Info("Entity: %s", entity->GetName().c_str()));
		PERSIST_OBJECT(ar, entity);
	}

	// 3. actors
	UINT actorCount = actors.size();
	PERSIST_FIELD(ar, actorCount);
	for (UINT i = 0; i < actorCount; ++i) {
		Actor* actor = actors[i];
		PERSIST_OBJECT(ar, actor);
	}

	// 4. animations

	// TODO

	// 5. material -> mesh relation
	StoreKeymap(ar, materials_to_meshes);
	
	// 6. entitiy -> actor relation
	StoreKeymap(ar, entities_to_actors);
	
	// 7. actor -> actor relation - scenegraph
	StoreKeymap(ar, actor_to_actor);

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

void StoreKeymap(Archive &ar, assoc_t &keymap){
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
