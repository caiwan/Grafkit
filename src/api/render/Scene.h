#pragma once
#include <stack>

#include "tree.h"

#include "../math/matrix.h"
#include "Actor.h"

#include "camera.h"
#include "Light.h"

#include "renderer.h"

#include "memory_align.h"

namespace Grafkit {

	//struct shader_pair_t {
	//	ShaderRef m_vertexShader;
	//	ShaderRef m_fragmentShader;

	//	shader_pair_t(ShaderRef vertexShader, ShaderRef fragmentShader) :
	//		m_vertexShader(vertexShader),
	//		m_fragmentShader(fragmentShader)
	//	{
	//	}

	//	shader_pair_t() :
	//		m_vertexShader(),
	//		m_fragmentShader()
	//	{}
	//};

#define SCENE_BUCKET "scene"

	__declspec(align(16)) class Scene : public IResource, public AlignedNew<Scene>
	// class Scene : public IResource 
	{
	public:
		Scene();
		~Scene();

		void PreRender(Grafkit::Renderer & render);
		void Render(Grafkit::Renderer & render);

		void SetRootNode(ActorRef &root) { m_pScenegraph = root; }

		// --- 

		CameraRef& GetCamera() { return this->m_rCamera; }

		void SetCameraNode(ActorRef &camera) { m_pCameraNode = camera; }
		void AddLightNode(ActorRef &light) { m_pvLightNodes.push_back(light); }

		std::vector<LightRef>& GetLights() { return this->m_rvLights; }

		Grafkit::Matrix& GetWorldMatrix() { return this->m_cureentViewMatrix; }

		ShaderRef &GetVShader() { return this->m_vertexShader; }
		ShaderRef &GetFShader() { return this->m_fragmentShader; }

		void SetVShader(ShaderRef &VS) {this->m_vertexShader = VS; }
		void SetFShader(ShaderRef &FS) {this->m_fragmentShader = FS; }

		virtual const char* GetBucketID() { return SCENE_BUCKET; }

	protected:
		ActorRef m_pScenegraph;
		// fenyek + camera

		ActorRef m_pCameraNode;
		std::vector<ActorRef> m_pvLightNodes;
		
		std::vector<LightRef> m_rvLights;
		CameraRef m_rCamera;

		//shader_pair_t m_shader;
		ShaderRef m_vertexShader;
		ShaderRef m_fragmentShader;

	private:
		void RenderNode(Grafkit::Renderer & render, Actor* actor, int maxdepth = TREE_MAXDEPTH);
		void push();
		void pop();

		Grafkit::Matrix m_cureentViewMatrix;
		std::stack<Grafkit::Matrix> m_viewMatrixStack; 

	private:
			
	};
}
