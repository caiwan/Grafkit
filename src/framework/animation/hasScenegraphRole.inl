#pragma once 
#include "scene.h"
#include "render/SceneGraph.h"

inline void Grafkit::HasSceneGraphRole::InitializeSceneGraph()
{
	m_scenegraph->Initialize();
}

inline void Grafkit::HasSceneGraphRole::ShutdownSceneGraph()
{
	m_scenegraph->Shutdown();
}

inline void Grafkit::HasSceneGraphRole::UpdateScenegraph()
{
	m_scenegraph->Update();
}

inline void Grafkit::HasSceneGraphRole::RenderScenegraph(Grafkit::Renderer & render, CameraRef & camera)
{
	m_scenegraph->Render(render, camera);
}

inline void Grafkit::HasSceneGraphRole::BuildSceneGraph(Grafkit::Renderer & render)
{
	m_scenegraph->BuildScene(render);
}