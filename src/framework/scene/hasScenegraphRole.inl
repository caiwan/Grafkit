#pragma once 
#include "render/SceneGraph.h"

inline void Grafkit::HasSceneGraphRole::InitializeSceneGraph()
{
    assert(m_scenegraph);
	m_scenegraph->Initialize();
}

inline void Grafkit::HasSceneGraphRole::ShutdownSceneGraph()
{
    assert(m_scenegraph);
    m_scenegraph->Shutdown();
}

inline void Grafkit::HasSceneGraphRole::UpdateScenegraph()
{
    assert(m_scenegraph);
	m_scenegraph->Update();
}

inline void Grafkit::HasSceneGraphRole::RenderScenegraph(Renderer & render, CameraRef & camera)
{
    assert(m_scenegraph);
	m_scenegraph->Render(render, camera);
}

inline void Grafkit::HasSceneGraphRole::BuildSceneGraph(Renderer & render)
{
    assert(m_scenegraph);
	m_scenegraph->BuildScene(render);
}