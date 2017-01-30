#pragma once

#include <stack>
#include <list>
#include <vector>

#include "../stdafx.h"

#include "renderer.h"
#include "Actor.h"
#include "camera.h"
#include "Light.h"
#include "animation.h"
// #include "../core/archetype.h"

namespace Grafkit {

	class Scene;
	typedef Ref<Scene> SceneRef;

	class Animation;
	typedef Ref<Animation> AnimationRef;

	__declspec(align(16)) 
	class Scene : virtual public Referencable, public AlignedNew<Scene>, public Persistent
	{
	public:
		Scene();
		~Scene();

		void Initialize(ActorRef root);
		void Shutdown();

		void PreRender(Grafkit::Renderer & render);
		void Render(Grafkit::Renderer & render);

		ActorRef& GetRootNode() { return m_pScenegraph; }

		// --- 
		void SetActiveCamera(std::string name);
		void SetActiveCamera(size_t id = 0) { m_activeCamera = m_cameraNodes[id]; }
		ActorRef GetActiveCamera() { return m_activeCamera; }

		size_t GetCameraCount() { return m_cameraNodes.size(); }
		ActorRef GetCamera(size_t id) { return m_cameraNodes[id]; }
		ActorRef GetCamera(std::string name);

		size_t GetLightCount() { return this->m_lightNodes.size(); }
		ActorRef GetLight(int n) { return this->m_lightNodes[n]; }
		ActorRef GetLight(std::string name);

		MaterialRef GetMaterial(std::string name);
		
		void AddAnimation(AnimationRef anim);
		void GetAnimations(std::vector<AnimationRef> &animations) { animations.clear(); animations.assign(m_animations.cbegin(), m_animations.cend()); }
		AnimationRef GetAnimation(int i) { return m_animations[i]; }
		void UpdateAnimation(double t) { m_animation_time = t; }

		Grafkit::Matrix& GetWorldMatrix() { return this->m_currentWorldMatrix; }

		ShaderRef &GetVShader() { return this->m_vertexShader; }
		ShaderRef &GetPShader() { return this->m_pixelShader; }

		void SetVShader(ShaderRef &VS) {this->m_vertexShader = VS; }
		void SetPShader(ShaderRef &FS) {this->m_pixelShader = FS; }

		void BuildScene(Grafkit::Renderer & deviceContext, ShaderRef vs, ShaderRef ps);

	private:
		ActorRef m_pScenegraph;

		ActorRef m_activeCamera;

		std::vector<ActorRef> m_cameraNodes;
		std::vector<ActorRef> m_lightNodes;

		std::set<Entity3D*> m_Entities;

		std::vector<AnimationRef> m_animations;

		double m_animation_time;

		ShaderRef m_vertexShader;
		ShaderRef m_pixelShader;

	private:
		void RenderNode(Grafkit::Renderer & render, Actor* actor, int maxdepth = 1024);
		void Push();
		void Pop();

		std::map<std::string, ActorRef> m_cameraMap;
		std::map<std::string, ActorRef> m_lightMap;
		std::map<std::string, ActorRef> m_nodeMap;

		std::map<std::string, MaterialRef> m_materialMap;

		Grafkit::Matrix m_cameraMatrix;

		Grafkit::Matrix m_currentWorldMatrix;
		std::stack<Grafkit::Matrix> m_worldMatrixStack; 
		
		Grafkit::Matrix m_cameraViewMatrix;
		Grafkit::Matrix m_cameraProjectionMatrix;

	private:
		Grafkit::Matrix CalcNodeTransformTree(ActorRef &actor);
	
	// -- persistent
	protected:
		virtual void serialize(Archive& ar);
		PERSISTENT_DECL(Grafkit::Scene, 1);
	};

	typedef Ref<Scene> SceneRef;
	typedef Resource<Scene> SceneRes;
	typedef Ref<SceneRes> SceneResRef;

}

