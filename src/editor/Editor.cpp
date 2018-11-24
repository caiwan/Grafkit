#include "stdafx.h"
#include "Editor.h"

#include "resource/ResourceManager.h"
#include "utils/asset/AssetFactory.h"

//#include "render/shader.h"
//#include "render/scene.h"
#include "scene/scene.h"

#include "demo.h"
#include "context.h"

#include "proxies/MusicProxy.h"
#include "schema.h"

using namespace Idogep;
using namespace GkDemo;
using namespace Grafkit;

Editor::Editor(Renderer& render, const std::weak_ptr<Context> & context)
    : Controller()
    , m_render(render)
    , m_precalcRequested(false)
    , m_reloadRequested(false)
    , m_isDirty(false)
    , m_tmpInitTestStuff(false)
    , m_context(context)
{
    m_commandStack = new CommandStack();
    m_musicProxy = new MusicProxy();
}

Editor::~Editor()
{
    delete m_musicProxy;
    delete m_commandStack;
}

void Editor::Initialize(IResourceManager* const& resman)
{
    m_myView = dynamic_cast<EditorView*>(View::SafeGetView(resman, "EditorView").Get());
    m_commandStack->onCommandStackChanged += Delegate(m_myView.Get(), &Roles::ManageCommandStackRole::CommandStackChangedEvent);

    m_myView->onUndo += Delegate(m_commandStack, &CommandStack::Undo);
    m_myView->onRedo += Delegate(m_commandStack, &CommandStack::Redo);

    //m_myView->onNew += Delegate(m_commandStack, &CommandStack::Redo);
    m_myView->onLoad += Delegate(this, &Editor::OpenDocument);
    m_myView->onSave += Delegate(this, &Editor::SaveDocument);
}

void Editor::InitializeDocument()
{
    auto context = m_context.lock();
    context->SetIsFxaa(true);
    Demo* demo = context->GetDemo()->Get();
    assert(demo);
    try
    {
        context->DoPrecalc();

        // TODO: remove it later on 

        //m_context->Intitialize();
        context->DoPrecalc();

        m_musicProxy->m_music = demo->GetMusic();

        onDocumentChanged(demo);
        m_musicProxy->onMusicChanged();
    }
    catch (FWdebug::Exception & ex)
    {
        MessageBoxA(nullptr, ex.what(), "Exception", 0);

        delete demo;
        context->SetDemo(nullptr);

        // TODO: Rollback stuff here or some shit 
    }

    m_commandStack->ClearStack();
}

bool Editor::RenderFrame()
{
    auto context = m_context.lock();
    Demo* demo = nullptr;
    if (context->GetDemo())
        demo = context->GetDemo()->Get();

    if (m_reloadRequested)
    {
        m_reloadRequested = false;
        InitializeDocument();
        return true;
    }

    if (m_precalcRequested)
    {
        context->DoPrecalc();
        m_precalcRequested = false;
        return true;
    }

    float time = m_musicProxy->GetTime();

    if (demo)
    {
        demo->PreRender(m_render, time);
        demo->Render(m_render, time);
    }

    if (m_musicProxy->IsPlaying())
        onDemoTimeChanged(time);

    // TODO: Bring this one here to the editor itself
    do {} while (context->GetAssetFactory()->PollEvents(context.get()));

    return true;
}

void Editor::NewDocument()
{
    // check shit here
    //m_context->CreateTestStuff();
    m_reloadRequested = true;
}

void Editor::SaveDocument() 
{
    //m_context->SaveCache();
    m_context.lock()->SaveSchema();
}

void Editor::OpenDocument() {
    assert(m_myView);
    if (m_isDirty)
    {
        // Todo: confirm save dialog
    }
    std::string path;
    if (m_myView->OpenFolder(path))
    {
        try
        {
            auto context = m_context.lock();
            context->Relocate(path);
            context->LoadDemo("schema.json");
            m_reloadRequested = true;
        }
        catch (FWdebug::Exception & ex)
        {
            MessageBoxA(nullptr, ex.what(), "Exception", 0);
        }
    }
}

bool Editor::DirtyCheck() const { return false; }

IResourceManager* Editor::GetResourceManager() const { return m_context.lock().get(); }
