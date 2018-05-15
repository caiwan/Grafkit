#include "stdafx.h"

#include "Environment.h"

// #include ""

#include "render/scene.h"

#include "render/Actor.h"
#include "render/Material.h"
#include "render/Scene.h"
#include "render/Light.h"
#include "render/model.h"
#include "render/camera.h"

#include "utils/resource.h"

using namespace Grafkit;
using namespace GKimporter;

GKimporter::SceneBuilder::SceneBuilder()
{
	scene = new Resource<Scene>(new Scene);
	rootActor = new Actor();
}

GKimporter::SceneBuilder::~SceneBuilder()
{
}

void GKimporter::SceneBuilder::Compose()
{
	(*scene)->Initialize(rootActor);
}

//void GKimporter::SceneBuilder::AddTexture(std::string name, Material::texture_type_e layer)
//{
//	// ... 
//}

void GKimporter::SceneBuilder::SetRootActor(Grafkit::ActorRef & actor)
{
	rootActor = actor;
}

void GKimporter::SceneBuilder::AddMaterial(MaterialRef & material)
{
	materialMap[material->GetName()] = material;
	materialList.push_back(material);
}

void GKimporter::SceneBuilder::AddModel(ModelRef & model)
{
	AddEntity(model.Get());
	modelMap[model->GetName()] = model;
	modelList.push_back(model);
}

void GKimporter::SceneBuilder::AddLight(LightRef & light)
{
	AddEntity(light.Get());
	lightMap[light->GetName()] = light;
	lightList.push_back(light);
}

void GKimporter::SceneBuilder::AddCamera(CameraRef & camera)
{
	AddEntity(camera.Get());
	cameraMap[camera->GetName()] = camera;
	cameraList.push_back(camera);
}

void GKimporter::SceneBuilder::AddActor(ActorRef & actor)
{
	actorMap[actor->GetName()] = actor;
	actorList.push_back(actor);
}

void GKimporter::SceneBuilder::AddAnimation(Grafkit::AnimationRef & animation)
{
	scene->Get()->AddAnimation(animation);
}

Grafkit::MaterialRef GKimporter::SceneBuilder::FindMaterial(std::string name)
{
	auto material = materialMap.find(name);
	if (material == materialMap.end())
		return Grafkit::MaterialRef();
	return material->second;
}

Grafkit::ModelRef GKimporter::SceneBuilder::FindModel(std::string name)
{
	Entity3DRef entity = FindEntity(name);
	return dynamic_cast<Model*>(entity.Get());
}

Grafkit::Entity3DRef GKimporter::SceneBuilder::FindEntity(std::string name)
{
	auto entity = entityMap.find(name);
	if (entity == entityMap.end())
		return Grafkit::Entity3DRef();
	return entity->second;
}

Grafkit::ActorRef GKimporter::SceneBuilder::FindActor(std::string name)
{
	auto actor = actorMap.find(name);
	if (actor == actorMap.end())
		return Grafkit::ActorRef();
	return actor->second;
}

Grafkit::CameraRef GKimporter::SceneBuilder::FindCamera(std::string name)
{
	auto camera = cameraMap.find(name);
	if (camera == cameraMap.end())
		return Grafkit::CameraRef();
	return camera->second;
}

Grafkit::LightRef GKimporter::SceneBuilder::FindLight(std::string name)
{
	auto light = lightMap.find(name);
	if (light == lightMap.end())
		return Grafkit::LightRef();
	return light->second;
}

Grafkit::MaterialRef GKimporter::SceneBuilder::FindMaterial(size_t id)
{
	return materialList[id];
}

Grafkit::ModelRef GKimporter::SceneBuilder::FindModel(size_t id)
{
	return modelList[id];
}

Grafkit::Entity3DRef GKimporter::SceneBuilder::FindEntity(size_t id)
{
	return entityList[id];
}

Grafkit::ActorRef GKimporter::SceneBuilder::FindActor(size_t id)
{
	return actorList[id];
}

void GKimporter::SceneBuilder::AddEntity(Entity3DRef entity)
{
	entityMap[entity->GetName()] = entity;
	entityList.push_back(entity);
}


// 

GKimporter::Environment::Environment() :
	builder()
	//, isLeftHanded(true)
{
}


GKimporter::Environment::~Environment()
{
}

