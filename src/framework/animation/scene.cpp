#include "scene.h"

#include "utils/exceptions.h"

#include "render/SceneGraph.h"
#include "render/light.h"
#include "render/camera.h"
#include "render/model.h"
#include "animation.h"

using namespace Grafkit;
using namespace FWdebugExceptions;


// ----------------------------------------------------------------------------

Grafkit::Scene::Scene() : HasSceneGraphRole(),
HasCamerasRole(), HasAnimationsRole(),
HasLightsRole()
{
}

Grafkit::Scene::~Scene()
{
	Shutdown();
}

void Grafkit::Scene::Initialize()
{
	if (m_scenegraph.Invalid())
		return;

	InitializeSceneGraph();
	InitializeCameras();
	InitializeLights();

	for (size_t i = 0; i < m_scenegraph->GetNodeCount(); i++) {
		ActorRef node = m_scenegraph->GetNode(i);

		for (int i = 0; i < node->GetEntityCount(); i++)
		{
			Entity3D * entity = node->GetEntity(i);

			// Lights
			LightRef light = dynamic_cast<Light*>(entity);
			if (light.Valid()) {
				AddLight(node, light);
				break; // We assume if we only have one under a node
			}

			// Cameras
			CameraRef  camera = dynamic_cast<Camera*>(entity);
			if (camera.Valid()) {
				AddCamera(node, camera);
				break; // assume if we have only one uder a node
			}

		}
	}

}

void Grafkit::Scene::Shutdown()
{
	ShutdownLights();
	ShutdownCameras();
	ShutdownSceneGraph();
}

void Grafkit::Scene::RenderFrame(Grafkit::Renderer & render, float time)
{
	UpdateScene(render, time);
    CameraRef camera = GetActiveCamera();
	RenderScenegraph(render, camera);
}


void Grafkit::Scene::Render(Grafkit::Renderer & render)
{
    CameraRef camera = GetActiveCamera();
	RenderScenegraph(render, camera);
	//m_scenegraph->Render(render, GetActiveCamera());
}

void Grafkit::Scene::serialize(Archive & ar)
{
}


void Grafkit::Scene::UpdateScene(Grafkit::Renderer & render, float time)
{
	UpdateAnimations(time);
	UpdateScenegraph();
	UpdateCamera(render);
	UpdateLights();
}

void Grafkit::Scene::Build(Grafkit::Renderer & render)
{
	BuildSceneGraph(render);
}

void Grafkit::Scene::Build(Grafkit::Renderer & render, ShaderResRef vs, ShaderResRef ps)
{
	m_scenegraph->SetPShader(ps);
	m_scenegraph->SetVShader(vs);
	BuildSceneGraph(render);

}

