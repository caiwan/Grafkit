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

		///@ todo kamerat, es fenyforrasokat itt kezelje(?)
		///@ accoarding to Gargaj: kell egy preframe process, ami kiszuli a kamerat es a fenyeket, majd azzal rendereli le a tobbi nodeot

		void Render(Grafkit::Renderer & render /*, ShaderRef &FS, ShaderRef &VS*/);
		void SetRootNode(Actor* root) { m_pScenegraph = root; }

		// --- 

		CameraRef& GetCamera() { return this->m_rCamera; }

		void SetCameraNode(Actor* camera);
		void AddLightNode(Actor* light);

		std::vector<LightRef>& GetLights() { return this->m_rvLights; }

		Grafkit::Matrix& GetWorldMatrix() { return this->m_cureentViewMatrix; }

		ShaderRef &GetVShader() { return this->m_vertexShader; }
		ShaderRef &GetFShader() { return this->m_fragmentShader; }

		void SetVShader(ShaderRef &VS) {this->m_vertexShader = VS; }
		void SetFShader(ShaderRef &FS) {this->m_fragmentShader = FS; }

		virtual const char* GetBucketID() { return SCENE_BUCKET; }

	protected:
		Actor* m_pScenegraph;
		// fenyek + camera

		Actor* m_pCameraNode;
		std::vector<Actor*> m_pvLightNodes;
		
		std::vector<LightRef> m_rvLights;
		CameraRef m_rCamera;

		//shader_pair_t m_shader;
		ShaderRef m_vertexShader;
		ShaderRef m_fragmentShader;

	private:
		void PreRender(Grafkit::Renderer & render);
		void RenderNode(Grafkit::Renderer & render, Actor* actor, int maxdepth = TREE_MAXDEPTH);
		void push();
		void pop();

		Grafkit::Matrix m_cureentViewMatrix;
		std::stack<Grafkit::Matrix> m_viewMatrixStack; 

	private:
			
	};
}
