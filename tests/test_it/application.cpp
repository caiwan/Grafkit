#include "stdafx.h"
#include "application.h"

#include "render/renderer.h"
#include "utils/asset/AssetFactory.h"

Testing::TestApplicationContext::TestApplicationContext(Grafkit::Renderer & render, std::unique_ptr<Grafkit::IAssetFactory> assetFactory)
    : m_render(render), m_assetFactory(std::move(assetFactory)), m_resourceManager(std::make_unique<Grafkit::ResourceManager>())
{
    InitializeWindows(320, 240);
    m_render.Initialize(m_window.getRealWidth(), m_window.getRealHeight(), true, this->m_window.getHWnd(), false);
}

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

