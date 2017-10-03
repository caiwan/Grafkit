#pragma once 

#include <vector>
#include <map>

#include "render/Material.h"

#include "common.h"

namespace GKimporter {

	// Good example of bad software design

	class SceneBuilder {
	public:
		SceneBuilder();
		~SceneBuilder();

		void Compose();

		Grafkit::SceneResRef GetScene() { return scene; }

		void SetRootActor(Grafkit::ActorRef &actor);

		//void AddTexture(std::string name, Grafkit::Material::texture_type_e layer);
		void AddMaterial(Grafkit::MaterialRef & material);
		void AddModel(Grafkit::ModelRef & model);
		void AddLight(Grafkit::LightRef & light);
		void AddCamera(Grafkit::CameraRef & camera);
		void AddActor(Grafkit::ActorRef & actor);

		void AddEntity(Grafkit::Entity3DRef entity);

		Grafkit::MaterialRef FindMaterial(std::string name);
		Grafkit::ModelRef FindModel(std::string name);
		Grafkit::Entity3DRef FindEntity(std::string name);
		Grafkit::ActorRef FindActor(std::string name);

		Grafkit::CameraRef FindCamera(std::string name);
		Grafkit::LightRef FindLight(std::string name);

		Grafkit::MaterialRef FindMaterial(size_t id);
		Grafkit::ModelRef FindModel(size_t id);
		Grafkit::Entity3DRef FindEntity(size_t id);
		Grafkit::ActorRef FindActor(size_t id);

		// add / manipulate animations


	protected:
		std::map<std::string, Grafkit::MaterialRef> materialMap;
		std::map<std::string, Grafkit::Entity3DRef> entityMap;
		std::map<std::string, Grafkit::ModelRef> modelMap;
		std::map<std::string, Grafkit::CameraRef> cameraMap;
		std::map<std::string, Grafkit::LightRef> lightMap;
		std::map<std::string, Grafkit::ActorRef> actorMap;

		std::vector<Grafkit::MaterialRef> materialList;
		std::vector<Grafkit::Entity3DRef> entityList;
		std::vector<Grafkit::ModelRef> modelList;
		std::vector<Grafkit::CameraRef> cameraList;
		std::vector<Grafkit::LightRef> lightList;
		std::vector<Grafkit::ActorRef> actorList;

		Grafkit::ActorRef rootActor;

		Grafkit::SceneResRef scene;
	};

	class Environment {
	public:
		Environment();
		~Environment();

		SceneBuilder& GetBuilder() { return builder; }

	private:
		SceneBuilder builder;
	};
}
