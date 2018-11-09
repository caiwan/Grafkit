#include "stdafx.h"
#include "application.h"

#include "render/renderer.h"
#include "utils/asset/AssetFactory.h"

Testing::TestApplicationContext::TestApplicationContext(Grafkit::Renderer& render, Grafkit::IAssetFactory* assetFactory)
    : m_render(render), m_assetFactory(assetFactory)
{
    InitializeWindows(320, 240);
    m_render.Initialize(m_window.getRealWidth(), m_window.getRealHeight(), true, this->m_window.getHWnd(), false);
}

Testing::TestApplicationContext::~TestApplicationContext() { Release(); }

int Testing::TestApplicationContext::Initialize() {
    m_render.BeginScene();
    m_render.EndScene();
    return 0;
}

int Testing::TestApplicationContext::Mainloop()
{
    m_render.BeginScene();
    m_render.EndScene();
    return 0;
}

void Testing::TestApplicationContext::Release()
{
    ShutdownWindows();
}

Grafkit::Renderer& Testing::TestApplicationContext::GetDeviceContext() { return m_render; }

Grafkit::IAssetFactory* Testing::TestApplicationContext::GetAssetFactory() { return m_assetFactory; }

