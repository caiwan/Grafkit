#pragma once

#include <stack>
#include <list>
#include <vector>

#include "common.h"

#include "math/matrix.h"

#include "renderer.h"

namespace Grafkit {
	__declspec(align(16))
		class SceneGraph : virtual public Referencable, public AlignedNew<SceneGraph>, public Persistent
	{
	public:
		SceneGraph();
		~SceneGraph();

		// Init, shutdown 
		void Initialize();
		void Initialize(ActorRef root); // legacy stuff 
		void Shutdown();

		void Update();
		void Render(Grafkit::Renderer & render, CameraRef & camera);

		// Add / Get node
		void AddNode(ActorRef& actor);
		ActorRef GetNode(std::string name);

		void SetRootNode(ActorRef &root) { m_root = root; }
		ActorRef GetRootNode() { return m_root; }

		size_t GetNodeCount() { return m_nodes.size(); }
		ActorRef GetNode(size_t id) { return m_nodes[id]; }

		// --- 

		Grafkit::Matrix& GetWorldMatrix() { return this->m_currentWorldMatrix; }

		ShaderResRef GetVShader() { return this->m_vertexShader; }
		ShaderResRef GetPShader() { return this->m_pixelShader; }

		void SetVShader(ShaderResRef &VS) { this->m_vertexShader = VS; }
		void SetPShader(ShaderResRef &FS) { this->m_pixelShader = FS; }

		void BuildScene(Grafkit::Renderer & render);
		void BuildScene(Grafkit::Renderer & render, ShaderResRef vs, ShaderResRef ps); //legacy stuff

	protected:

		// we'll see if we need this or not 
		// or if we have to move it into another layer 

		struct WorldMatrices_t {
			matrix worldMatrix;
			matrix viewMatrix;
			matrix projectionMatrix;
		};

		struct WorldMatrices_t GetWorldMatrices() { return m_worldMatrices; }

	protected:

		ActorRef m_root;

		struct WorldMatrices_t m_worldMatrices;

		ShaderResRef m_vertexShader;
		ShaderResRef m_pixelShader;

		std::vector<ActorRef> m_nodes;
		std::map<std::string, ActorRef> m_nodeMap;

		std::set<Entity3D*> m_entities;

	private:
		double m_tAnim;

	private:
		Grafkit::Matrix m_currentWorldMatrix;
		std::stack<Grafkit::Matrix> m_worldMatrixStack;

		// -- persistent
	protected:
		virtual void serialize(Archive& ar);
		PERSISTENT_DECL(Grafkit::SceneGraph, 1);
	};
}
