#include "stdafx.h"

#include "SceneGraph.h"

#include "Actor.h"
#include "model.h"
#include "camera.h"
#include "Light.h"
#include "shader.h"

#include "dxtypes.h"

PERSISTENT_IMPL(Grafkit::SceneGraph);

using namespace Grafkit;
using namespace FWdebugExceptions;

SceneGraph::SceneGraph() : Object()
, m_root(nullptr)
//, m_tAnim(0) 
{
}

//SceneGraph::~SceneGraph() { Shutdown(); }

void SceneGraph::Initialize()
{
    m_nodes.clear();
    m_nodeMap.clear();

    std::stack <std::pair<ActorRef, ActorRef>> stack; // node, parent
    stack.push(std::make_pair(m_root, nullptr));

    while (!stack.empty())
    {
        auto nodePair = stack.top();
        auto &node = nodePair.first;
        stack.pop();

        if (nodePair.first)
        {
            m_nodes.push_back(nodePair);
            m_nodeMap[node->GetName()] = node;
        }

        for (int i = 0; i < node->GetEntityCount(); ++i) { m_entities.insert(node->GetEntity(i)); }

        for (int i = 0; i < node->GetChildrenCount(); i++) { stack.push(std::make_pair(node->GetChild(i), node)); }
    }
}

void SceneGraph::Initialize(ActorRef root)
{
    m_root = root;
    Initialize();
}

void SceneGraph::BuildScene(Renderer& render) { std::for_each(m_entities.begin(), m_entities.end(), [&](auto & entity) { entity->Build(render, *this); }); }

void SceneGraph::BuildScene(Renderer& render, const ShaderRes& vs, const ShaderRes& ps) 
{
    assert(vs && ps);
    m_vertexShader = vs;
    m_pixelShader = ps;
    BuildScene(render);
}

/******************************************************************************
 * RENDER
 *****************************************************************************/

void SceneGraph::Update()
{
    for (size_t i = 0; i < m_nodes.size(); i++)
    {
        const ActorRef& actor = m_nodes[i].first;
        const ActorRef& parent = m_nodes[i].second;
        const Matrix& matrix = actor->Matrix();
        const Matrix& transform = actor->Transform();

        Matrix world = actor->WorldMatrix();
        if (!parent)
            world.Identity();
        else
            world = parent->WorldMatrix();
        world.Multiply(matrix);
        world.Multiply(transform);
        actor->WorldMatrix(world);
    }
}

void SceneGraph::Render(Renderer& render, const CameraRef& camera)
{
    m_worldMatrices.viewMatrix = XMMatrixTranspose(camera->GetWorldMatrix().Get());
    m_worldMatrices.projectionMatrix = XMMatrixTranspose(camera->GetPerspectiveMatrix().Get());

    // erre lehet nincs szukseg kulonosebben
    m_currentWorldMatrix.Identity();
    m_worldMatrices.worldMatrix = XMMatrixTranspose(m_currentWorldMatrix.Get());

    m_vertexShader->Bind(render);
    m_pixelShader->Bind(render);

    int vsMatrixId = m_vertexShader->GetParamId("MatrixBuffer");
    int psMatrixId = m_pixelShader->GetParamId("MatrixBuffer");

    // render scenegraph
    for (size_t i = 0; i < m_nodes.size(); i++)
    {
        ActorRef& actor = m_nodes[i].first;
        m_worldMatrices.worldMatrix = XMMatrixTranspose(actor->WorldMatrix().Get());
        m_vertexShader->SetParamT(render, vsMatrixId, m_worldMatrices);
        m_pixelShader->SetParamT(render, psMatrixId, m_worldMatrices);

        if (!actor->IsHidden()) { actor->Render(render, *this); }
    }

    m_vertexShader->Unbind(render);
    m_pixelShader->Unbind(render);
}

ActorRef SceneGraph::GetNode(const std::string & name)
{
    auto it = m_nodeMap.find(name);
    if (it != m_nodeMap.end())
        return it->second;
    return nullptr;
}
