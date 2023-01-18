#include "utils/ResourceManager.h"

#include "render/shader.h"
//#include "render/scene.h"
#include "animation/scene.h"

#include "Editor.h"
#include "models/Document.h"

#include "proxies/MusicProxy.h"

using namespace Idogep;

Editor::Editor(Ref<Controller> parent, Grafkit::Renderer& render, Grafkit::IResourceManager* const & resman)
    : Controller(parent)
  , m_resourceManager(resman)
  , m_render(render)
  , m_document(nullptr)
  , m_precalcRequested(false)
  , m_reloadRequested(false)
{
    m_commandStack = new CommandStack();
    m_musicProxy = new MusicProxy();
}

Editor::~Editor()
{
    delete m_musicProxy;
    delete m_commandStack;
}

void Editor::Initialize()
{
    //NewDocument();
    //InitializeDocument();
}

void Editor::InitializeDocument()
{
    try
    {
        m_document->Preload(m_resourceManager);
        m_resourceManager->DoPrecalc();
        m_document->Initialize(m_render);

        m_musicProxy->m_music = m_document->m_music;

        onDocumentChanged(m_document);
        m_musicProxy->onMusicChanged();

        //m_musicProxy->Play();
    }
    catch (FWdebug::Exception* ex)
    {
        MessageBoxA(nullptr, ex->what(), "Exception", 0);

        delete m_document;
        m_document = nullptr;

        delete ex;

        // TODO: Rollback stuff here
    }
}

bool Editor::RenderFrame()
{
    // preload // reload
    if (m_reloadRequested)
    {
        m_reloadRequested = false;
        InitializeDocument();
        return true;
    }

    if (m_precalcRequested)
    {
        m_resourceManager->DoPrecalc();
        m_precalcRequested = false;
        return true;
    }

    // --- 

    m_render.BeginScene();

    if (m_document)
    {
        float t = m_musicProxy->GetTime();

        m_document->m_rootActor->Matrix().Identity();
        //m_document->m_rootActor->Matrix().RotateRPY(0, 0, 0);
        m_document->m_rootActor->Matrix().Translate(0, 0, 0);

        m_document->m_cameraActor->Matrix().Identity();
        m_document->m_cameraActor->Matrix().RotateRPY(M_PI / 4., 0, M_PI / 4.);
        m_document->m_cameraActor->Matrix().Translate(0, 0, 10);

        (*m_document->m_scene)->RenderFrame(m_render, t);
    }

    m_render.EndScene();

    // ---
    return true; // keep it infinite for now
}

void Editor::NewDocument()
{
    // check shit here

    m_document = new Document();
    m_reloadRequested = true;
}

// ReSharper disable CppMemberFunctionMayBeStatic
// ReSharper disable CppMemberFunctionMayBeConst
void Editor::SaveDocument()
{
}

void Editor::OpenDocument(std::string filename)
{
}
// ReSharper restore CppMemberFunctionMayBeConst

bool Editor::DirtyCheck() const { return false; }
// ReSharper restore CppMemberFunctionMayBeStatic

Grafkit::IResourceManager* Editor::GetResourceManager() const { return m_resourceManager; }

