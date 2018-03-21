#pragma once 

#include <stack>
#include <list>
#include <vector>

#include "common.h"

#include "render/camera.h"
#include "render/light.h"
#include "render/SceneGraph.h"

#include "animation.h"

namespace Grafkit {

	class SceneRenderBuffers;
	class CameraContainer;
	class LightContainer;
	//class GizmoContainer;

	class Scene : virtual public Referencable, public Persistent
	{
	public:
		Scene();
		~Scene();

		//void Precalc();

		void Initialize(ActorRef root);
		void Shutdown();

		// Render stuff 
		void RenderFrame(Grafkit::Renderer & render, float time);

		//void PreRender(Grafkit::Renderer & render);
		void Render(Grafkit::Renderer & render, Ref<SceneRenderBuffers> & target);
		 
		void AddAnimation(AnimationRef anim);
		void AddNode(ActorRef& node);

		void GetAnimations(std::vector<AnimationRef> &animations) { animations.clear(); animations.assign(m_animations.cbegin(), m_animations.cend()); }
		AnimationRef GetAnimation(int i) { return m_animations[i]; }

		void UpdateScene(Grafkit::Renderer & render);
		void UpdateAnimation(double t);

		//bool IsActive() { return true && (m_tAnim >= m_tStart && m_tAnim < m_tEnd); }

		void Build(Grafkit::Renderer & render, ShaderResRef vs, ShaderResRef ps);

	protected:
		 std::vector<AnimationRef> m_animations;

		 CameraContainer *m_cameras;
		 LightContainer *m_lights;
		 SceneRenderBuffers *m_outputBuffers;

		 SceneGraph *m_scenegraph;
	};

	// ---------------------------------------------------------------------------
	// A helper class to handle multiple cameras within a scene
	class CameraContainer {
	public:
		CameraContainer();
		~CameraContainer();

		void AddCamera(ActorRef &node, CameraRef &camera);
		size_t GetCameraCount() { return m_cameras.size(); }
		ActorRef GetCamera(size_t id) { return m_cameras[id].actor; }
		ActorRef GetCamera(std::string name);

		// automation track for camera
		void AddCameraFrame(float t, std::string name);

		void SetActiveCamera(std::string name);
		void SetActiveCamera(size_t id) { m_activeCamera = m_cameras[id]; }

		ActorRef GetActiveCameraNode() { return m_activeCamera.actor; }
		CameraRef GetActiveCamera() { return m_activeCamera.camera; }

		void Update(Grafkit::Renderer & render);

	private:
		struct camera_t {
			camera_t() {}

			ActorRef actor;
			CameraRef camera;
			size_t id;
		};

		camera_t m_activeCamera;

		std::vector<camera_t> m_cameras;
		std::map<std::string, camera_t> m_cameraMap;
	};

	// --- 

	// ---------------------------------------------------------------------------
	// A helper class to handle multiple light sources within a scene

	class LightContainer {
	public:
		LightContainer();
		~LightContainer();

		void AddLight(ActorRef &node, LightRef &light); 

		size_t GetLightCount() { return this->m_lights.size(); }
		ActorRef GetLight(int n) { return this->m_lights[n].actor; }
		ActorRef GetLight(std::string name);

		// tobbi fiszfaszka is kellene majd ide 

		void Update(Grafkit::Renderer & render);

	private:

		struct lightData_t {
			lightData_t() {}

			union {
				int lightCount;
				float4 _;
			};
		} m_lightData;

		struct light_t {
			light_t() {}

			ActorRef actor;
			LightRef light;
			size_t id;
		};

		std::vector<light_t> m_lights;
		std::map<std::string, light_t> m_lightMap;

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
