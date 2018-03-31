#pragma once 

#include <stack>
#include <list>
#include <vector>

#include "common.h"

#include "render/Actor.h"
#include "render/SceneGraph.h"

#include "render/Light.h"
#include "render/Camera.h"

#include "animation.h"

namespace Grafkit {

	class SceneRenderBuffers;
	class CameraContainer;
	class LightContainer;

	class EntityContainer {
	public:
		EntityContainer() {}
		virtual ~EntityContainer() {}

		inline void AddEntiy(ActorRef node, Entity3DRef entity);
		size_t GetCount() { return m_entities.size(); }
		ActorRef GetActor(size_t id) { return m_entities[id].m_actor; }
		inline ActorRef GetActor(std::string name);

		Entity3DRef GetEntity(size_t id) { return m_entities[id].m_entity; }
		inline Entity3DRef GetEntity(std::string name);

		inline size_t GetId(std::string name);

		inline void Calculate(Renderer &render, size_t id);
		inline void CalculateAll(Renderer &render);

	protected:
		struct entity_t {
			entity_t() {}
			entity_t(ActorRef &actor, Entity3DRef &entity, size_t id) : m_actor(actor), m_entity(entity), m_id(id) {}

			ActorRef m_actor;
			Entity3DRef m_entity;
			size_t m_id;
		};

		std::vector<entity_t> m_entities;
		std::map<std::string, entity_t> m_entityMap;
	};

	// --- 
	// Separate varoius responsibilites of a monolyth class
	class HasSceneGraphRole {
	public:
		HasSceneGraphRole() {}
		virtual ~HasSceneGraphRole() {}

		void SetSceneGraph(SceneGraphRef &scenegraph) { m_scenegraph = scenegraph; }
		SceneGraphRef GetSceneGraph() { return m_scenegraph; }

	protected:
		inline void InitializeSceneGraph();
		inline void ShutdownSceneGraph();

		inline void UpdateScenegraph();
		inline void RenderScenegraph(Grafkit::Renderer & render, CameraRef & camera);
		inline void BuildSceneGraph(Grafkit::Renderer & deviceContext);

	protected:
		SceneGraphRef m_scenegraph;
	};

	// --- 

	class HasAnimationsRole {

	public:
		HasAnimationsRole() {}
		virtual ~HasAnimationsRole() {}

		void AddAnimation(AnimationRef animation) { m_animations.push_back(animation); }

		size_t GetAnimationCount() { return m_animations.size(); }
		AnimationRef GetAnimation(size_t id) { return m_animations[id]; }

	protected:
		void InitializeAnimations() {}
		void ShutdownAnimations(){}


		void UpdateAnimations(float t);

		std::vector<AnimationRef> m_animations;

	};

	// --- 

	class HasCamerasRole {
	public:
		HasCamerasRole();
		virtual ~HasCamerasRole();

		inline void AddCamera(ActorRef &actor, CameraRef &camera);

		size_t GetCameraId(std::string name) { m_cameras->GetId(name); }

		void SetActiveCameraId(size_t id) { m_activeCameraId = id; }
		size_t GetActiveCameraId() { return m_activeCameraId; }

		ActorRef GetActiveCameraActor() { return m_cameras->GetActor(m_activeCameraId); }
		CameraRef GetActiveCamera() { return dynamic_cast<Camera*>(m_cameras->GetEntity(m_activeCameraId).Get()); }

		ActorRef GetCameraActor(std::string name) { return m_cameras->GetActor(name); }
		ActorRef GetCameraActor(size_t id) { return m_cameras->GetActor(id); }
		
		CameraRef GetCamera(std::string name) { return dynamic_cast<Camera*>(m_cameras->GetEntity(name).Get()); }
		CameraRef GetCamera(size_t id) { return dynamic_cast<Camera*>(m_cameras->GetEntity(id).Get()); }

		void AddCameraFrame(float time, std::string name);
		void AddCameraFrame(float time, size_t id);

	protected:
		void InitializeCameras();
		void ShutdownCameras();

		void UpdateCamera(Renderer & render);

	protected:
		EntityContainer * m_cameras;
		size_t m_activeCameraId;
	};

	// --- 

	class HasLightsRole {
	public:
		void AddLight(ActorRef &actor, LightRef &camera){}

	protected:

		void InitializeLights(){}
		void ShutdownLights(){}

		void UpdateLights(){}

		EntityContainer * m_lights;
	};



	// ---------------------------------------------------------------------------


	class Scene : virtual public Referencable, public Persistent,
		public HasSceneGraphRole,
		public HasAnimationsRole,
		public HasCamerasRole,
		public HasLightsRole
	{
	public:
		Scene();
		virtual ~Scene();

		void Initialize();
		void Shutdown();

		void Build(Grafkit::Renderer & render);
		void Build(Grafkit::Renderer & render, ShaderResRef vs, ShaderResRef ps); // legacy stuff

		void UpdateScene(Grafkit::Renderer & render, float time);

		void RenderFrame(Grafkit::Renderer & render, float time);
		void Render(Grafkit::Renderer & render);


		// -- persistent
	protected:
		virtual void serialize(Archive& ar);
		PERSISTENT_DECL(Grafkit::Scene, 1);
	};


	// ---------------------------------------------------------------------------
	// 
	class SceneRenderBuffers {
	public:
		//void Precalc(/* + resman */); // + resman

		// + Initialize
		// + Shutdown

		// + bind to render
		// + bind to shader param

	private:
		Texture2DResRef m_mapColor;
		Texture2DResRef m_mapNormal;
		Texture2DResRef m_mapPosition;
		Texture2DResRef m_mapHeight;
		Texture2DResRef m_mapRoughness;
		Texture2DResRef m_mapMetallic;
		Texture2DResRef m_mapEmission;
	};

}


// ---------------------------------------------------------------------------
// inline implementation
// ---------------------------------------------------------------------------

#include "EntityContainer.inl"

#include "hasScenegraphRole.inl"
#include "hasAnimationsRole.inl"
#include "hasCamerasRole.inl"
