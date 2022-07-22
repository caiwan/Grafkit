#include "scene.h"

#include "core/exceptions.h"

#include "render/SceneGraph.h"
#include "render/light.h"
#include "render/camera.h"
#include "render/model.h"
//#include "render/shader.h"
#include "animation/animation.h"

using namespace Grafkit;
using namespace FWdebugExceptions;

// =============================================================================================

inline void Grafkit::HasAnimationsRole::UpdateAnimations(float t)
{
    for (size_t i = 0; i < m_animations.size(); i++)
    {
        AnimationRef & animation = m_animations[i];
        animation->Update(t);
    }
}

// =============================================================================================

Scene::Scene() : HasSceneGraphRole()
    , HasAnimationsRole()
    , HasCamerasRole()
    , HasLightsRole() {
}

void Scene::Initialize()
{
    assert(m_scenegraph);

    InitializeSceneGraph();

    for (size_t i = 0; i < m_scenegraph->GetNodeCount(); i++)
    {
        ActorRef node = m_scenegraph->GetNode(i);

        for (int j = 0; j < node->GetEntityCount(); j++)
        {
            Entity3DRef entity = node->GetEntity(j);

            // Lights
            LightRef light = std::dynamic_pointer_cast<Light>(entity);
            if (light)
            {
                AddLight(node, light);
                break; // We assume if we only have one under a node
            }

            // Cameras
            CameraRef camera = std::dynamic_pointer_cast<Camera>(entity);
            if (camera)
            {
                AddCamera(node, camera);
                break; // assume if we have only one uder a node
            }
        }
    }
}

void Scene::RenderFrame(Renderer& render, float time)
{
    UpdateScene(render, time);
    CameraRef camera = GetActiveCamera();
    RenderScenegraph(render, camera);
}

void Scene::Render(Renderer& render)
{
    CameraRef camera = GetActiveCamera();
    RenderScenegraph(render, camera);
}

void Scene::UpdateScene(Renderer& render, float time)
{
    UpdateAnimations(time);
    UpdateScenegraph();
    UpdateCamera(render);
    UpdateLights(render);
}

void Scene::Build(Renderer& render) { BuildSceneGraph(render); }

void Scene::Build(Renderer& render, ShaderResRef vs, ShaderResRef ps)
{
    m_scenegraph->SetPShader(ps);
    m_scenegraph->SetVShader(vs);
    BuildSceneGraph(render);
}
