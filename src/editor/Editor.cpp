#include "utils/ResourceManager.h"

#include "render/shader.h"
//#include "render/scene.h"
#include "animation/scene.h"

#include "Editor.h"
#include "models/Document.h"

#include "proxies/MusicProxy.h"

using namespace Idogep;

Idogep::Editor::Editor(Ref<Module> parent, Grafkit::Renderer & render, Grafkit::IResourceManager * const & resman) : Module(parent),
	m_render(render),
	m_resourceManager(resman), m_document(nullptr), m_reloadRequested(false), m_precalcRequested(false)
{
	m_commandStack = new CommandStack();
	m_musicProxy = new MusicProxy();
}

Idogep::Editor::~Editor()
{
	delete m_musicProxy;
	delete m_commandStack;
}

void Idogep::Editor::Initialize()
{
	//NewDocument();
	//InitializeDocument();
}

void Idogep::Editor::InitializeDocument()
{
	try {
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
		MessageBoxA(NULL, ex->what(), "Exception", 0);

		delete m_document;
		m_document = nullptr;

		delete ex;
	}

}

bool Idogep::Editor::RenderFrame()
{
	// preload // reload
	if (m_reloadRequested) {
		m_reloadRequested = false;
		InitializeDocument();
		return true;
	}

	if (m_precalcRequested) {
		m_resourceManager->DoPrecalc();
		m_precalcRequested = false;
		return true;
	}

	// --- 

	m_render.BeginScene();

	if (m_document)
	{
		static float t;
		t += .001;
		m_document->m_rootActor->Matrix().Identity();
		//m_document->m_rootActor->Matrix().RotateRPY(0, 0, 0);
		m_document->m_rootActor->Matrix().Translate(0, 0, 0);

		m_document->m_cameraActor->Matrix().Identity();
		m_document->m_cameraActor->Matrix().RotateRPY(M_PI / 4., 0, M_PI / 4.);
		m_document->m_cameraActor->Matrix().Translate(0, 0, 10);

		(*m_document->m_scene)->RenderFrame(m_render, 0.0f);

	}

	m_render.EndScene();

	// ---
	return true; // keep it infinite for now
}

void Idogep::Editor::NewDocument()
{
	// check shit here

	m_document = new Document();
	m_reloadRequested = true;
}

void Idogep::Editor::SaveDocument()
{
}

void Idogep::Editor::OpenDocument(std::string filename)
{
}

Grafkit::IResourceManager * Idogep::Editor::GetResourceManager()
{
	return m_resourceManager;
}

bool Idogep::Editor::DirtyCheck()
{
	return false;
}
