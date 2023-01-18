#include "stdafx.h"

#include "SceneGraph.h"
#include "Actor.h"
#include "model.h"
#include "camera.h"
#include "Light.h"

PERSISTENT_IMPL(Grafkit::SceneGraph);

using namespace Grafkit;
using namespace FWdebugExceptions;

SceneGraph::SceneGraph() :
	m_root(nullptr)
{
}

SceneGraph::~SceneGraph()
{
	Shutdown();
}

void SceneGraph::Initialize()
{
	std::stack<ActorRef> stack;
	stack.push(m_root);

	while (!stack.empty()) {
		ActorRef node = stack.top(); stack.pop();

		AddNode(node);

		for (int i = 0; i < node->GetChildrenCount(); i++) {
			stack.push(node->GetChild(i));
		}
	}
}

void SceneGraph::Initialize(ActorRef root)
{
	m_root = root;
	Initialize();
}

void SceneGraph::Shutdown()
{
	m_nodeMap.clear();

	m_entities.clear();

	m_root = nullptr;

	m_pixelShader = nullptr;
	m_vertexShader = nullptr;

}

void SceneGraph::BuildScene(Renderer & render) {
	
	for (auto it = m_entities.begin(); it != m_entities.end(); ++it) {
		(*it)->Build(render, this);
	}
}

void SceneGraph::BuildScene(Renderer & render, ShaderResRef vs, ShaderResRef ps)
{
	if (vs.Valid())
		m_vertexShader = vs;

	if (ps.Valid())
		m_pixelShader = ps;
	BuildScene(render);


}

/******************************************************************************
 * RENDER
 *****************************************************************************/

void SceneGraph::Update()
{
	for (size_t i = 0; i < m_nodes.size(); i++) {
		const ActorRef &actor = m_nodes[i];
		const ActorRef &parent = actor->GetParent();
		const Matrix &matrix = actor->Matrix();
		const Matrix &transform = actor->Transform();

		Matrix world = actor->WorldMatrix();
		if (parent.Invalid())
			world.Identity();
		else
			world = parent->WorldMatrix();
		world.Multiply(matrix);
		world.Multiply(transform);
		actor->WorldMatrix(world);
	}
}

void SceneGraph::Render(Renderer & render, CameraRef & camera)
{
	m_worldMatrices.viewMatrix = XMMatrixTranspose(camera->GetWorldMatrix().Get());
	m_worldMatrices.projectionMatrix = XMMatrixTranspose(camera->GetPerspectiveMatrix().Get());

	// erre lehet nincs szukseg kulonosebben
	m_currentWorldMatrix.Identity();
	m_worldMatrices.worldMatrix = XMMatrixTranspose(m_currentWorldMatrix.Get());

	Shader *ps = m_pixelShader->Get();
	Shader *vs = m_vertexShader->Get();

	vs->Bind(render);
	ps->Bind(render);

	int vsMatrixId = vs->GetParamId("MatrixBuffer");
	int psMatrixId = ps->GetParamId("MatrixBuffer");

	// render scenegraph
	for (size_t i = 0; i < m_nodes.size(); i++)
	{
		ActorRef & actor = m_nodes[i];
		m_worldMatrices.worldMatrix = XMMatrixTranspose(actor->WorldMatrix().Get());
		vs->SetParamT(render, vsMatrixId, m_worldMatrices);
		ps->SetParamT(render, psMatrixId, m_worldMatrices);

		if (!actor->IsHidden()) {
			actor->Render(render, this);
		}
	}

	vs->Unbind(render);
	ps->Unbind(render);
}

void SceneGraph::AddNode(ActorRef & node)
{
	if (node.Invalid())
		return;

	m_nodes.push_back(node);
	m_nodeMap[node->GetName()] = node;

	for (int i = 0; i < node->GetEntityCount(); i++)
	{
		m_entities.insert(node->GetEntity());
	}

}

ActorRef SceneGraph::GetNode(std::string name)
{
	auto it = m_nodeMap.find(name);
	if (it != m_nodeMap.end())
		return it->second;
	return nullptr;
}

void SceneGraph::Serialize(Archive & ar)
{
	// actorok, entityk exportja, importja goez here 
	// a regi scene loaderbol ki kell szedni es ide be kell rakosgatni
    assert(0);
}
