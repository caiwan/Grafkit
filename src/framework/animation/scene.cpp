#include "scene.h"

#include "utils/exceptions.h"

#include "render/SceneGraph.h"
#include "render/light.h"
#include "render/camera.h"
#include "render/model.h"
#include "animation.h"

using namespace Grafkit;
using namespace FWdebugExceptions;

Grafkit::Scene::Scene() : m_cameras(nullptr), m_lights(nullptr), m_outputBuffers(nullptr), m_scenegraph(nullptr)
{
	m_outputBuffers = new SceneRenderBuffers();
}

Grafkit::Scene::~Scene()
{
	Shutdown();
}

void Grafkit::Scene::Initialize(ActorRef root)
{
	m_cameras = new CameraContainer();
	m_lights = new LightContainer();
	m_scenegraph = new SceneGraph();
	m_scenegraph->Initialize(root);

	for (size_t i = 0; i< m_scenegraph->GetNodeCount(); i++ ){
		ActorRef node = m_scenegraph->GetNode(i);
		AddNode(node);
	}

}

void Grafkit::Scene::Shutdown()
{
	delete m_cameras;
	delete m_lights;
	delete m_scenegraph;
}

void Grafkit::Scene::RenderFrame(Grafkit::Renderer & render, float time)
{
}


void Grafkit::Scene::Render(Grafkit::Renderer & render, Ref<SceneRenderBuffers> & target)
{
	CameraRef camera = m_cameras->GetActiveCamera();
	m_scenegraph->Render(render, camera);
}

void Grafkit::Scene::AddAnimation(AnimationRef anim)
{
	m_animations.push_back(anim);
}

void Grafkit::Scene::AddNode(ActorRef & node)
{
	if (node.Invalid())
		return;

	m_scenegraph->AddNode(node);

	for (int i = 0; i < node->GetEntityCount(); i++)
	{
		Entity3D * entity = node->GetEntity(i);

		// Lights
		LightRef light = dynamic_cast<Light*>(entity);
		if (light.Valid()) {
			m_lights->AddLight(node, light);
			break; // We assume if we only have one under a node
		}

		// Cameras
		CameraRef  camera = dynamic_cast<Camera*>(entity);
		if (camera.Valid()) {
			m_cameras->AddCamera(node, camera);
			break; // assume if we have only one uder a node
		}

	}
}

void Grafkit::Scene::UpdateScene(Grafkit::Renderer & render)
{
	m_scenegraph->PreRender(render);
	m_lights->Update(render);
	m_cameras->Update(render);

}

void Grafkit::Scene::UpdateAnimation(double t)
{
	for (size_t i = 0; i < m_animations.size(); i++) {
		AnimationRef &animation = m_animations[i];
		animation->Update(t);
	}
}

void Grafkit::Scene::Build(Grafkit::Renderer & render, ShaderResRef vs, ShaderResRef ps)
{
	m_scenegraph->BuildScene(render, vs, ps);
}






Grafkit::CameraContainer::CameraContainer()
{
}

Grafkit::CameraContainer::~CameraContainer()
{
}

void Grafkit::CameraContainer::AddCamera(ActorRef & node, CameraRef & camera)
{
	if (node.Invalid() || camera.Invalid())
		return;
	camera_t record;
	record.actor = node;
	record.camera = camera;
	record.id = m_cameras.size();
	m_cameras.push_back(record);
	m_cameraMap[node->GetName()] = record;
}

ActorRef Grafkit::CameraContainer::GetCamera(std::string name)
{
	auto it = m_cameraMap.find(name);
	if (it != m_cameraMap.end())
		return it->second.actor;
	return ActorRef();
}

void Grafkit::CameraContainer::SetActiveCamera(std::string name)
{
	auto it = m_cameraMap.find(name);
	if (it != m_cameraMap.end())
		m_activeCamera = it->second;
}

void Grafkit::CameraContainer::Update(Grafkit::Renderer & render)
{
	// --- camera
	if (m_activeCamera.actor.Valid() && m_activeCamera.camera.Valid())
	{
		CameraRef & camera = m_activeCamera.camera;
		ActorRef & cameraActor = m_activeCamera.actor;
		camera->Calculate(render, cameraActor);
	}
	else {
		throw new EX_DETAILS(NullPointerException, "Camera actor nem jo, vagy Nem seteltel be a nodeba kamerat");
	}
}

Grafkit::LightContainer::LightContainer()
{
}

Grafkit::LightContainer::~LightContainer()
{
}

void Grafkit::LightContainer::AddLight(ActorRef & node, LightRef & light)
{
}

ActorRef Grafkit::LightContainer::GetLight(std::string name)
{
	auto it = m_lightMap.find(name);
	if (it != m_lightMap.end())
		return it->second.actor;
	return ActorRef();
}

void Grafkit::LightContainer::Update(Grafkit::Renderer & render)
{
	// --- lights
	m_lightData.lightCount = m_lights.size();
	for (size_t i = 0; i < m_lights.size(); ++i)
	{
		light_t & l = m_lights[i];
		//Matrix &wm = l.actor->WorldMatrix();
		l.light->Calculate(render, l.actor);

		// -- majd::
		//l.light->GetInternalData(&m_lightData.lights[i]);
	}
}
