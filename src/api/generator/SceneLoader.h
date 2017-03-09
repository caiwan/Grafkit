#pragma once

#include <map>
#include <set>
#include <vector>

#include "utils/exceptions.h"

#include "utils/persistence/persistence.h"

#include "utils/asset.h"
#include "utils/resource.h"
#include "utils/ResourceBuilder.h"
			
#include "render/predefs.h"

namespace Grafkit {

	class SceneLoader : public Grafkit::IResourceBuilder
	{
	public:
		SceneLoader(std::string name, std::string source_name);
		~SceneLoader();

		void Load(Grafkit::IResourceManager * const & resman, Grafkit::IResource * source);

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

			void Load(Archive &ar, IResourceManager * const & resman);
			void Save(Archive &ar);

		public:
			
			typedef std::pair<USHORT, USHORT> assoc_t;
			typedef std::pair<std::string, std::string> texture_bind_t;

			typedef std::map<UINT, std::vector<texture_bind_t>> texture_assoc_t;

		private:
		
			// --- 
			void BuildObjectMaps();
			void BuildTextureMap(const MaterialRef & material);
			void BuildMaterialMap(const ModelRef &model);
			void BuildEntityMap(const ActorRef &actor);
			void BuildActorMap();
			void BuildAnimationMap();

			// --- 
			void Persist(Archive &ar, IResourceManager * const & resman);
			void PersistMaterials(Archive &ar, IResourceManager * const & resman);
			void PersistTextures(Archive &ar, MaterialRef &material, IResourceManager * const & resman);
			void PersistEntities(Archive &ar, IResourceManager * const & resman);
			void PersistActors(Archive &ar, IResourceManager * const & resman);
			void PersistAnimations(Archive &ar, IResourceManager * const & resman);

			void PersistKeymap(Archive &ar, std::vector<assoc_t> &keymap);

		private:
			SceneRef &m_scene;

			std::vector<Actor*> m_actors;
			std::vector<Entity3D*> m_entities;
			std::vector<Material*> m_materials;
			std::vector<Animation*> m_Animations;

			std::map<Entity3D*, USHORT> m_entity_map;
			std::set<Material*> m_material_set;
			std::map<Actor*, int> m_actor_map;

			texture_assoc_t m_textures_to_materials;
			std::vector<assoc_t> m_materials_to_meshes;
			std::vector<assoc_t> m_entities_to_actors;
			std::vector<assoc_t> m_actor_to_actor;
			std::vector<assoc_t> m_animation_to_actor;
			std::vector<assoc_t> m_animation_to_entity;
			
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
