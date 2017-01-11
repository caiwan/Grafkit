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
typedef  std::map<int, std::list<int>> assoc_t;

void Grafkit::SceneLoader::Load(Grafkit::IResourceManager * const & assman, Grafkit::IResource * source)
{
	SceneResRef dstScene = dynamic_cast<SceneRes*>(source);
	if (dstScene.Invalid()) {
		throw EX(NullPointerException);
	}

	IAssetRef asset = this->GetSourceAsset(assman);

	ArchiveMemory ar((BYTE*)asset->GetData(), asset->GetSize());

	SceneRef scene = dynamic_cast<Scene*>(Scene::load(ar));

	if (scene.Invalid())
		throw EX(SceneLoadException);
	
	// --- 
	std::map<int, Material*> materials;
	std::map<int, Entity3D*> entities;
	std::map<int, Actor*> actors;
	std::map<int, Animation*> animations;

	assoc_t materials_to_meshes;
	assoc_t entities_to_actors;
	assoc_t scenegraph;

	// 1. material_map
	// 2. entity_map
	// 3. actors
	// 4. animations

	// 5. material -> mesh relation
	// 6. entitiy -> actor relation
	// 7. actor -> actor relation - scenegraph

	dstScene->AssingnRef(scene);
}

void Grafkit::SceneLoader::Save(SceneRes scene, std::string dst_name)
{
	FILE *fp = nullptr;
	fopen_s(&fp, dst_name.c_str(), "wb");
	if (!fp) throw EX_DETAILS(SceneLoadException, "cannot serialize object");

	ArchiveFile ar(fp, true);

	ar & PERSIST_OBJECT(scene);

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
				}
				++i; // actor count
			}
		}
		// </yield>
		
		for (auto it = node->GetChildren().begin(); it != node->GetChildren().end(); it++)
			stack.push(*it);
	}

	for (size_t i = 0; i < actors.size(); ++i) {
		auto parent = actor_map.find(actors[i]);
		if (parent != actor_map.end()) {
			actor_to_actor[i].push_back(parent->second);
		}
	}
	
	// --- persist scene 

	// 1. materials
	size_t materialCount = materials.size();
	ar & PERSIST_FIELD(materialCount);
	for (size_t i = 0; i < materialCount; ++i) {
		Material *& material = materials[i];
		ar & PERSIST_OBJECT(material);
	}

	// 2. entity
	size_t entityCount = entities.size();
	ar & PERSIST_FIELD(entityCount);
	for (size_t i = 0; i < entityCount; ++i) {
		ar & PERSIST_OBJECT(entities[i]);
	}

	// 3. actors
	size_t actorCount = actors.size();
	ar & PERSIST_FIELD(actorCount);
	for (size_t i = 0; i < actorCount; ++i) {
		ar & PERSIST_OBJECT(actors[i]);
	}

	// 4. animations

	// TODO

	// 5. material -> mesh relation
	
	/*size_t materialMapCount = materials_to_meshes.size()
	for (auto it )*/

	// 6. entitiy -> actor relation
	
	// 7. actor -> actor relation - scenegraph

	fflush(fp);
	fclose(fp);

}
