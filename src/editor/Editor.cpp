#include "Editor.h"

#include "utils/ResourceManager.h"

#include "render/shader.h"
//#include "render/scene.h"
#include "experimental/scene.h"

#include "demo.h"
#include "context.h"

#include "proxies/MusicProxy.h"

using namespace Idogep;
using namespace GkDemo;
using namespace Grafkit;

Editor::Editor(Renderer& render, Context* const& context)
    : Controller()
    , m_render(render)
    , m_precalcRequested(false)
    , m_reloadRequested(false)
    , m_isDirty(false)
    , m_demo(nullptr)
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

    NewDocument(); // Emit, or put the end of the event queue somehow?
}

void Editor::InitializeDocument()
{
    try
    {
        m_demo->Preload(m_context);
        m_context->DoPrecalc();

        m_demo->Initialize(m_render);

        m_musicProxy->m_music = m_demo->GetMusic();

        onDocumentChanged(m_demo);
        m_musicProxy->onMusicChanged();

    //m_musicProxy->Play();
    }
    catch (FWdebug::Exception* ex)
    {
        MessageBoxA(nullptr, ex->what(), "Exception", 0);

        delete m_demo;
        m_demo = nullptr;

        delete ex;

    // TODO: Rollback stuff here
    }
}

bool Editor::RenderFrame()
{
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

    if (m_demo)
        m_demo->Render(m_render, time);

    if (m_musicProxy->IsPlaying())
        onDemoTimeChanged(time);

    return true;
}

void Editor::NewDocument()
{
    // check shit here

    m_demo = new Demo();
    m_reloadRequested = true;

}

// ReSharper disable CppMemberFunctionMayBeStatic
// ReSharper disable CppMemberFunctionMayBeConst
void Editor::SaveDocument() {
}

void Editor::OpenDocument(std::string filename) {
}

// ReSharper restore CppMemberFunctionMayBeConst

bool Editor::DirtyCheck() const { return false; }
// ReSharper restore CppMemberFunctionMayBeStatic

IResourceManager* Editor::GetResourceManager() const { return m_context; }

//IResourceManager* Editor::GetApplicationResourceManager() const { return m_resourceManager; }
