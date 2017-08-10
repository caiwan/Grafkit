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
		void SetActiveCamera(size_t id = 0) { m_activeCamera = m_cameraNodes[id]; }

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
		void GetAnimations(std::vector<AnimationRef> &animations) { animations.clear(); animations.assign(m_animations.cbegin(), m_animations.cend()); }
		AnimationRef GetAnimation(int i) { return m_animations[i]; }
		void UpdateAnimation(double t) { m_tAnim = t; }

		Grafkit::Matrix& GetWorldMatrix() { return this->m_currentWorldMatrix; }

		void AddMaterialLayer(UINT layer, ShaderRef &shader) { m_materialShaderMap[layer] = shader; }
		unsigned int GetLayerID() { return m_materialCurrentLayer; }

		ShaderRef &GetVShader() { return this->m_vertexShader; }
		ShaderRef &GetPShader() { return this->m_pixelShader; }

		void SetVShader(ShaderRef &VS) {this->m_vertexShader = VS; }
		void SetPShader(ShaderRef &FS) {this->m_pixelShader = FS; }

		float GetStartTime() { return m_tStart; }
		float GetEndTime()   { return m_tEnd; }
		
		void GetStartTime(float start) { m_tStart = start; }
		void GetEndTime(float end) {m_tEnd = end; }

		bool IsActive() { return true && (m_tAnim >= m_tStart && m_tAnim < m_tEnd); }

		void BuildScene(Grafkit::Renderer & deviceContext, ShaderRef vs, ShaderRef ps);

	private:
		float m_tStart, m_tEnd;

		ActorRef m_root;
		ActorRef m_activeCamera;

		// Todo: these neeeds to be optimiyed and tied up a bit
		std::vector<ActorRef> m_cameraNodes;
		std::vector<ActorRef> m_lightNodes;

		// This one as well
		std::set<Entity3D*> m_entities;

		std::vector<AnimationRef> m_animations;

		double m_tAnim;

		ShaderRef m_vertexShader;
		ShaderRef m_pixelShader;

	private:
		void RenderLayer(Grafkit::Renderer & render, UINT layer);
		void PrerenderNode(Grafkit::Renderer & render, Actor* actor, int maxdepth = 1024);
		void Push();
		void Pop();

		std::map<std::string, ActorRef> m_cameraMap;
		std::map<std::string, ActorRef> m_lightMap;
		std::map<std::string, ActorRef> m_nodeMap;

		std::list<ActorRef> m_nodes;

		std::map<std::string, MaterialRef> m_materialMap;
		std::map<UINT, ShaderRef> m_materialShaderMap;
		UINT m_materialCurrentLayer;

		Grafkit::Matrix m_cameraMatrix;

		Grafkit::Matrix m_currentWorldMatrix;
		std::stack<Grafkit::Matrix> m_worldMatrixStack; 
		
		Grafkit::Matrix m_cameraViewMatrix;
		Grafkit::Matrix m_cameraProjectionMatrix;
	
	// -- persistent
	protected:
		virtual void serialize(Archive& ar);
		PERSISTENT_DECL(Grafkit::Scene, 1);
	};
}

