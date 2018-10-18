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

Testing::TestApplicationContext::~TestApplicationContext() { ShutdownWindows(); }

int Testing::TestApplicationContext::init() {
    m_render.BeginScene();
    m_render.EndScene();
    return 0;
}

int Testing::TestApplicationContext::mainloop()
{
    m_render.BeginScene();
    m_render.EndScene();
    return 0;
}

Grafkit::Renderer& Testing::TestApplicationContext::GetDeviceContext() { return m_render; }

Grafkit::IAssetFactory* Testing::TestApplicationContext::GetAssetFactory() { return m_assetFactory; }

