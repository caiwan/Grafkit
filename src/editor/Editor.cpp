#include "Editor.h"

#include "utils/ResourceManager.h"

#include "render/shader.h"
//#include "render/scene.h"
#include "experimental/scene.h"

#include "demo.h"
#include "context.h"

#include "proxies/MusicProxy.h"
#include "schema.h"

using namespace Idogep;
using namespace GkDemo;
using namespace Grafkit;

Editor::Editor(Renderer& render, Context* const& context)
    : Controller()
    , m_render(render)
    , m_precalcRequested(false)
    , m_reloadRequested(false)
    , m_isDirty(false)
    , m_tmpInitTestStuff(false)
    //, m_demo(nullptr)
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
    Demo* demo = m_context->GetDemo();
    assert(demo);
    try
    {
        //demo->Preload(m_context);
        m_context->DoPrecalc();

        // TODO: remove it later

        m_context->Intitialize();

        m_musicProxy->m_music = demo->GetMusic();

        onDocumentChanged(demo);
        m_musicProxy->onMusicChanged();

        //m_musicProxy->Play();
    }
    catch (FWdebug::Exception & ex)
    {
        MessageBoxA(nullptr, ex.what(), "Exception", 0);

        delete demo;
        m_context->SetDemo(nullptr);

        // TODO: Rollback stuff here or some shit 
    }
}

bool Editor::RenderFrame()
{
    Demo* demo = m_context->GetDemo();

    if (m_reloadRequested)
    {
        m_reloadRequested = false;
        InitializeDocument();
        return true;
    }

    if (m_precalcRequested)
    {
        m_context->DoPrecalc();
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

    return true;
}

void Editor::NewDocument()
{
    // check shit here
    m_context->CreateTestStuff();
    m_reloadRequested = true;
}

void Editor::SaveDocument() {

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
            m_context->Relocate(path);
            m_context->LoadScema();
            m_reloadRequested = true;
        }
        catch (FWdebug::Exception & ex)
        {
            MessageBoxA(nullptr, ex.what(), "Exception", 0);
        }
    }
}

bool Editor::DirtyCheck() const { return false; }

IResourceManager* Editor::GetResourceManager() const { return m_context; }
