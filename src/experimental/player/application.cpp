#include "application.h"
#include "demo.h"
#include "context.h"

#include "core/Music.h"

#include "utils/asset/AssetFile.h"
#include "resource/ResourceManager.h"


using namespace GrafkitData;
using namespace Grafkit;
using namespace GkDemo;

Application::Application(fwzSettings& settings) : System()
    , LoaderBar()
    , m_context(nullptr)
{
    m_file_loader = new FileAssetFactory("./");
    m_context = new Context(m_render, m_file_loader);
    m_context->SetIsFxaa(settings.nMultisample);
    m_context->SetPreloadListener(this);

    int screenWidth = settings.scrWidth;
    int screenHeight = settings.scrHeight;

    //m_hasfxaa = settings.nMultisample;

    InitializeWindows(screenWidth, screenHeight, !settings.nWindowed, 0, "Grafkit2 Player");

    screenWidth = m_window.getRealWidth() , screenHeight = m_window.getRealHeight();

    int result = 0;
    result = m_render.Initialize(screenWidth, screenHeight, settings.nVsync, m_window.getHWnd(), !settings.nWindowed);
    assert(result);

    this->m_render.SetViewportAspect(2.33, 1.);
}

Application::~Application()
{
    delete m_context;
    delete m_file_loader;
    m_render.Shutdown();
}

int Application::init()
{
    InitializeLoaderBar(m_render);

    m_context->LoadCache();
    m_context->LoadScema();
    m_context->DoPrecalc();
    m_context->Intitialize();
    m_demo = m_context->GetDemo();
    assert(m_demo);
    m_music = m_demo->GetMusic();
    assert(m_music);
    assert(*m_music);
    (*m_music)->Play();
    return 0;
}

int Application::mainloop()
{
    float time = (*m_music)->GetTime();
    m_demo->PreRender(m_render, time);
    m_demo->Render(m_render, time);
    (*m_music)->Update();

    int res = 0;
    res |= !(*m_music)->IsPlaying();
    res |= isEscPressed();
    return res;
}

void Application::release()
{
    if (m_music && *m_music)
        (*m_music)->Stop();
}

void Application::UpdateLoaderBar(float p) { DrawLoaderBar(m_render, p); }
