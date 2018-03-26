#pragma once 
#include "scene.h"
#include "render/camera.h"


inline Grafkit::HasCamerasRole::HasCamerasRole() : m_activeCameraId(0), m_cameras(nullptr)
{
}

inline void Grafkit::HasCamerasRole::AddCamera(ActorRef & actor, CameraRef & camera)
{
	m_cameras->AddEntiy(actor, dynamic_cast<Entity3D*>(camera.Get()));
}


inline Grafkit::HasCamerasRole::~HasCamerasRole()
{
}


inline void Grafkit::HasCamerasRole::AddCameraFrame(float time, std::string name)
{
	AddCameraFrame(time, m_cameras->GetId(name));
}

inline void Grafkit::HasCamerasRole::AddCameraFrame(float time, size_t id)
{
}

inline void Grafkit::HasCamerasRole::InitializeCameras()
{
	m_cameras = new EntityContainer();
}

inline void Grafkit::HasCamerasRole::ShutdownCameras()
{
	delete m_cameras;
	m_cameras = nullptr;
}

inline void Grafkit::HasCamerasRole::UpdateCamera(Renderer & render)
{
	m_cameras->Calculate(render, m_activeCameraId);
}
