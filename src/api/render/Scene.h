#pragma once
#include <stack>
#include "../math/matrix.h"
#include "Actor.h"
#include "renderer.h"
#include "tree.h"

namespace Grafkit {
	class Scene //: virtual public Referencable
	{
	public:
		Scene();
		~Scene();

		///@ todo kamerat, es fenyforrasokat itt kezelje(?)
		///@ accoarding to Gargaj: kell egy preframe process, ami kiszuli a kamerat es a fenyeket, majd azzal rendereli le a tobbi nodeot

		void Render(Grafkit::Renderer & render);

		void SetRootNode(Actor* root) { m_pScenegraph = root; }

	protected:
		Actor* m_pScenegraph;
		// fenyek + camera

		Actor* m_pCameraNode;
		std::vector<Actor*> m_pvLightNodes;
		
		//std::vector<LightRef> m_rvLights;
		CameraRef m_rCamera;

	private:
		void PreRender(Grafkit::Renderer & render);
		void RenderNode(Grafkit::Renderer & render, Actor* actor, int maxdepth = TREE_MAXDEPTH);
		void push();
		void pop();

		FWmath::Matrix m_cureentViewMatrix;
		std::stack<FWmath::Matrix> m_viewMatrixStack; 

	private:
			
	};
}
