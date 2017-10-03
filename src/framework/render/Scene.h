#pragma once

#include <stack>
#include <list>
#include <vector>

#include "math/matrix.h"

#include "Light.h"
#include "Camera.h"

#include "renderer.h"
#include "animation.h"
#include "predefs.h"

namespace Grafkit {
	__declspec(align(16))
		class Scene : virtual public Referencable, public AlignedNew<Scene>, public Persistent
	{
	public:
		Scene();
		~Scene();

		// Init, shutdown 
		void Initialize(ActorRef root);
		void Shutdown();

		// Render stuff 
		void RenderFrame(Grafkit::Renderer & render, float time) {
			UpdateAnimation(time);
			PreRender(render);
			Render(render);
		}

		void PreRender(Grafkit::Renderer & render);
		void Render(Grafkit::Renderer & render);

		// Add / Get node
		void AddNode(ActorRef& actor);
		ActorRef GetNode(std::string name);

		ActorRef& GetRootNode() { return m_root; }

		// --- 

		//void AddCamera(ActorRef camera);
		size_t GetCameraCount() { return m_cameras.size(); }
		ActorRef GetCamera(size_t id) { return m_cameras[id].actor; }
		ActorRef GetCamera(std::string name);

		void AddCameraFrame(float t, std::string name);

		void SetActiveCamera(std::string name);
		void SetActiveCamera(size_t id) { m_activeCamera = m_cameras[id]; }

		ActorRef GetActiveCameraNode() { return m_activeCamera.actor; }
		CameraRef GetActiveCamera() { return m_activeCamera.camera; }
		// 

		size_t GetLightCount() { return this->m_lights.size(); }
		ActorRef GetLight(int n) { return this->m_lights[n].actor; }
		ActorRef GetLight(std::string name);

		// MaterialRef GetMaterial(std::string name);

		//
		void AddAnimation(AnimationRef anim);

		void GetAnimations(std::vector<AnimationRef> &animations) { animations.clear(); animations.assign(m_animations.cbegin(), m_animations.cend()); }
		AnimationRef GetAnimation(int i) { return m_animations[i]; }

		void UpdateAnimation(double t) { m_tAnim = t; }

		Grafkit::Matrix& GetWorldMatrix() { return this->m_currentWorldMatrix; }

		ShaderRef GetVShader() { return this->m_vertexShader->Get(); }
		ShaderRef GetPShader() { return this->m_pixelShader->Get(); }

		void SetVShader(ShaderResRef &VS) { this->m_vertexShader = VS; }
		void SetPShader(ShaderResRef &FS) { this->m_pixelShader = FS; }

		float GetStartTime() { return m_tStart; }
		float GetEndTime() { return m_tEnd; }

		void SetStartTime(float start) { m_tStart = start; }
		void SetEndTime(float end) { m_tEnd = end; }

		bool IsActive() { return true && (m_tAnim >= m_tStart && m_tAnim < m_tEnd); }

		void BuildScene(Grafkit::Renderer & deviceContext, ShaderResRef vs, ShaderResRef ps);

	public:

		struct WorldMatrices_t {
			matrix worldMatrix;
			matrix viewMatrix;
			matrix projectionMatrix;
		};

		struct WorldMatrices_t GetWorldMatrices() { return m_worldMatrices; }

	protected:
		float m_tStart, m_tEnd;

		ActorRef m_root;
		Grafkit::Matrix m_cameraViewMatrix;
		Grafkit::Matrix m_cameraProjectionMatrix;
		Grafkit::Matrix m_cameraMatrix;

		std::vector<AnimationRef> m_animations;

		ShaderResRef m_vertexShader;
		ShaderResRef m_pixelShader;

		std::list<ActorRef> m_nodes;
		std::map<std::string, ActorRef> m_nodeMap;

		struct camera_t {
			camera_t() {}

			ActorRef actor;
			CameraRef camera;
			size_t id;
		};

		camera_t m_activeCamera;
		Animation::Track<int> m_activecameraTrack;

		std::vector<camera_t> m_cameras;
		std::map<std::string, camera_t> m_cameraMap;

		struct lightData_t {
			lightData_t() {}

			Light::light2_t lights[16];
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

		// std::map<std::string, MaterialRef> m_materialMap;

		std::set<Entity3D*> m_entities;

		struct WorldMatrices_t m_worldMatrices;

	private:
		double m_tAnim;

	private:
		void PrerenderNode(Grafkit::Renderer & render, Actor* actor, int maxdepth = 1024);
		void Push();
		void Pop();

	private:
		Grafkit::Matrix m_currentWorldMatrix;
		std::stack<Grafkit::Matrix> m_worldMatrixStack;

		// -- persistent
	protected:
		virtual void serialize(Archive& ar);
		PERSISTENT_DECL(Grafkit::Scene, 1);
	};
}

