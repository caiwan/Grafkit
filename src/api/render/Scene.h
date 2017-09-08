#pragma once

#include <stack>
#include <list>
#include <vector>

#include "math/matrix.h"

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

		void Initialize(ActorRef root);
		void Shutdown();

		void RenderFrame(Grafkit::Renderer & render, float time) {
			UpdateAnimation(time);
			PreRender(render);
			Render(render);
		}

		void PreRender(Grafkit::Renderer & render);
		void Render(Grafkit::Renderer & render);

		ActorRef& GetRootNode() { return m_root; }

		// --- 
		void SetActiveCamera(std::string name);
		void SetActiveCamera(size_t id) { m_activeCamera = m_cameraNodes[id]; }

		void SetActiveCameraFrame(float t) { int k = GetCurrentCameraFrame(t); if (k>=0) m_activeCamera = m_cameraNodes[k]; }

		ActorRef GetActiveCamera() { return m_activeCamera; }

		size_t GetCameraCount() { return m_cameraNodes.size(); }
		ActorRef GetCamera(size_t id) { return m_cameraNodes[id]; }
		ActorRef GetCamera(std::string name);

		void AddCamera(ActorRef camera);

		size_t GetLightCount() { return this->m_lightNodes.size(); }
		ActorRef GetLight(int n) { return this->m_lightNodes[n]; }
		ActorRef GetLight(std::string name);

		MaterialRef GetMaterial(std::string name);

		void AddAnimation(AnimationRef anim);

		void AddCurrentCameraFrame(float t, int id) { m_activecameraTrack.AddKey(Animation::Key<int>(t, id)); }
		int GetCurrentCameraFrame(float t) {
			int i0 = -1, i1 = -1; float k = 0;
			if (m_activecameraTrack.FindKey(t, i0, i1, k))
				return i0;
			return -1;
		}

		void GetAnimations(std::vector<AnimationRef> &animations) { animations.clear(); animations.assign(m_animations.cbegin(), m_animations.cend()); }
		AnimationRef GetAnimation(int i) { return m_animations[i]; }
		//void GetAnimations(ActorRef & actor, std::vector<AnimationRef> &animations);

		void UpdateAnimation(double t) { m_tAnim = t; }

		Grafkit::Matrix& GetWorldMatrix() { return this->m_currentWorldMatrix; }

		//void AddMaterialLayer(UINT layer, ShaderRef &shader) { m_materialShaderMap[layer] = shader; }
		//unsigned int GetLayerID() { return m_materialCurrentLayer; }

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
		// qnd hack of shit
		struct WorldMatrices_t {
			matrix worldMatrix;
			matrix viewMatrix;
			matrix projectionMatrix;
		};

		struct WorldMatrices_t GetWorldMatrices() { return m_worldMatrices; }

	protected:
		float m_tStart, m_tEnd;

		// TODO: the datamodel for scenegraph shold be handled here,
		// and objects should be accessed by their proper getter/setter
		// unlike in assimploader
		// + revew all the stuff stored here

		ActorRef m_root;
		Grafkit::Matrix m_cameraViewMatrix;
		Grafkit::Matrix m_cameraProjectionMatrix;
		Grafkit::Matrix m_cameraMatrix;

		ActorRef m_activeCamera;
		Animation::Track<int> m_activecameraTrack;

		std::vector<AnimationRef> m_animations;

		ShaderResRef m_vertexShader;
		ShaderResRef m_pixelShader;

		std::vector<ActorRef> m_cameraNodes;
		std::vector<ActorRef> m_lightNodes;

		std::map<std::string, ActorRef> m_cameraMap;
		std::map<std::string, ActorRef> m_lightMap;
		std::map<std::string, ActorRef> m_nodeMap;

		std::list<ActorRef> m_nodes;

		std::map<std::string, MaterialRef> m_materialMap;

		// This one as well
		std::set<Entity3D*> m_entities;

	private:
		double m_tAnim;

	private:
		void RenderLayer(Grafkit::Renderer & render, UINT layer);
		void PrerenderNode(Grafkit::Renderer & render, Actor* actor, int maxdepth = 1024);
		void Push();
		void Pop();

	private:
		//std::map<UINT, ShaderRef> m_materialShaderMap;
		//UINT m_materialCurrentLayer;

		struct WorldMatrices_t m_worldMatrices;

		//Grafkit::Matrix m_currentCameraMatrix;
		Grafkit::Matrix m_currentWorldMatrix;
		std::stack<Grafkit::Matrix> m_worldMatrixStack;

		// -- persistent
	protected:
		virtual void serialize(Archive& ar);
		PERSISTENT_DECL(Grafkit::Scene, 1);
	};
}

