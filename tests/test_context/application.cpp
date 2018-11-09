#include "stdafx.h"
#include "application.h"

#include "context.h"

#include "render/renderer.h"
#include "utils/asset/AssetFactory.h"
#include "utils/asset/AssetFile.h"

using namespace Grafkit;
using namespace GkDemo;

Testing::TestApplicationContext::TestApplicationContext()
{
    m_assetFactory = std::make_shared<FileAssetFactory>("tests/assets/");
    m_context = std::make_unique<Context>(m_render, m_assetFactory);
    InitializeWindows(320, 240);
    m_render.Initialize(m_window.getRealWidth(), m_window.getRealHeight(), true, this->m_window.getHWnd(), false);
}

Testing::TestApplicationContext::~TestApplicationContext() { /*Boops?*/ }

int Testing::TestApplicationContext::Initialize()
{
    m_render.BeginSceneDev();
    m_render.EndScene();
    return 0;
}

int Testing::TestApplicationContext::Mainloop()
{
    m_render.BeginSceneDev();
    m_render.EndScene();
    return 0;
}

void Testing::TestApplicationContext::Release() {
    ShutdownWindows();
}
