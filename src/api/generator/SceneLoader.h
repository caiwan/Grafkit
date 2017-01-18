#pragma once

#include "utils/exceptions.h"

#include "utils/persistence/persistence.h"

#include "utils/asset.h"
#include "utils/resource.h"
#include "utils/ResourceBuilder.h"
			
#include "render/Scene.h"
#include "render/model.h"
#include "render/texture.h"

namespace Grafkit {

	class SceneLoader : public Grafkit::IResourceBuilder
	{
	public:
		SceneLoader(std::string name, std::string source_name);
		~SceneLoader();

		void Load(Grafkit::IResourceManager * const & assman, Grafkit::IResource * source);

		static void Save(SceneRes scene, std::string dst_name);

		virtual IResource* NewResource() { return new SceneRes(); }

	protected:
		/* 
		* A QnD helper class to aid scene persistence
		*/
		class SceneLoaderHelper {
		public:
			SceneLoaderHelper(Archive &ar, SceneRef &scene);
			~SceneLoaderHelper();

			void Load(Archive &ar);
			void Store(Archive &ar);

		public:
			typedef std::map<USHORT, std::vector<USHORT>> assoc_t;

			typedef struct {
				std::string name;
				std::string bind;
			} texture_bind_t;

			typedef std::map<UINT, std::vector<texture_bind_t>> texture_assoc_t;

		private:
			// --- Load stuff 

			void LoadKeymap(Archive &ar, SceneLoader::SceneLoaderHelper::assoc_t &keymap);

			// --- Store stuff
			void BuildObjectMaps();
			void BuildTextureMap(const MaterialRef & material);
			void BuildMaterialMap(const ModelRef &model);
			void BuildEntityMap(const ActorRef &actor);
			void BuildActorMap();

			void StoreMaterials(Archive &ar);
			void StoreEntities(Archive &ar);
			void StoreActors(Archive &ar);
			void StoreAnimations(Archive &ar);

			void StoreKeymap(Archive &ar, SceneLoader::SceneLoaderHelper::assoc_t &keymap);

		private:
			SceneRef &m_scene;

			std::vector<Actor*> m_actors;
			std::vector<Entity3D*> m_entities;
			std::vector<Material*> m_materials;

			std::map<Entity3D*, int> m_entity_map;
			std::set<Material*> m_material_set;
			std::map<Actor*, int> m_actor_map;

			texture_assoc_t m_textures_to_materials;
			assoc_t m_materials_to_meshes;
			assoc_t m_entities_to_actors;
			assoc_t m_actor_to_actor;
			
			// used for crsr of the fetch loops
			int m_cTexID;
			int m_cMatID;
			int m_cEntityID;
			int m_cActorID;
		};
	};
}

// exceptions
DEFINE_EXCEPTION(SceneLoadException, 4000, "Error deserialize scene data");
