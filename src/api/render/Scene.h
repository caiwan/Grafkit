#pragma once

#include <stack>

#include "../stdafx.h"

#include "renderer.h"
#include "Actor.h"
#include "camera.h"
#include "Light.h"

namespace Grafkit {

	class Scene;
	typedef Ref<Scene> SceneRef;

#define SCENE_BUCKET "scene"

	__declspec(align(16)) class Scene : virtual public Referencable, public AlignedNew<Scene>
	{
	public:
		Scene();
		~Scene();

		void PreRender(Grafkit::Renderer & render);
		void Render(Grafkit::Renderer & render);

		void SetRootNode(ActorRef root) { m_pScenegraph = root; }
		ActorRef& GetRootNode() { return m_pScenegraph; }

		// --- 
		
		void AddCameraNode(ActorRef camera);
		void SetActiveCamera(size_t id = 0) { m_activeCamera = m_cameraNodes[id]; }
		ActorRef GetActiveCamera() { return m_activeCamera; }

		size_t GetCameraCount() { return m_cameraNodes.size(); }
		ActorRef GetCamera(size_t id) { return m_cameraNodes[id]; }

		void AddLightNode(ActorRef light);

		ActorRef GetLight(int n) { return this->m_lightNodes[n]; }
		size_t GetLightCount() { return this->m_lightNodes.size(); }

		Grafkit::Matrix& GetWorldMatrix() { return this->m_currentWorldMatrix; }

		ShaderRef &GetVShader() { return this->m_vertexShader; }
		ShaderRef &GetFShader() { return this->m_fragmentShader; }

		void SetVShader(ShaderRef &VS) {this->m_vertexShader = VS; }
		void SetFShader(ShaderRef &FS) {this->m_fragmentShader = FS; }

		virtual const char* GetBucketID() { return SCENE_BUCKET; }

	protected:
		ActorRef m_pScenegraph;
		// fenyek + camera

		ActorRef m_activeCamera;
		std::vector<ActorRef> m_cameraNodes;
		std::vector<ActorRef> m_lightNodes;

		ShaderRef m_vertexShader;
		ShaderRef m_fragmentShader;

	private:
		void RenderNode(Grafkit::Renderer & render, Actor* actor, int maxdepth = 1024);
		void push();
		void pop();

		Grafkit::Matrix m_cameraMatrix;

		Grafkit::Matrix m_currentWorldMatrix;
		std::stack<Grafkit::Matrix> m_worldMatrixStack; 
		
		Grafkit::Matrix m_cameraViewMatrix;
		Grafkit::Matrix m_cameraProjectionMatrix;

	private:
		Grafkit::Matrix CalcNodeTransformTree(ActorRef &actor);
	};

	typedef Ref<Scene> SceneRef;
	typedef Resource<Scene> SceneRes;
	typedef Ref<SceneRes> SceneResRef;
}
