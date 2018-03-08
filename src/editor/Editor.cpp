#include "Editor.h"
#include "Document.h"

#include "MusicProxy.h"

using namespace Idogep;

Idogep::Editor::Editor(Grafkit::Renderer & render, Grafkit::IResourceManager * const & resman) : 
	m_render(render), 
	m_resourceManager(resman), m_document(nullptr)
{
	m_musicProxy = new MusicProxy();
	m_commandStack = new CommandStack();
}

Idogep::Editor::~Editor()
{
	delete m_musicProxy;
	delete m_commandStack;
}

void Idogep::Editor::Initialize()
{
	// ... 
}

bool Idogep::Editor::RenderFrame()
{
	// preload // reload
	if (m_reloadRequested) {
		m_reloadRequested = false;
		//m_resourceManager->Dop //?
		Initialize(); //?
		return true;
	}

	if (m_precalcRequested) {
		m_precalcRequested = false;
		return true;
	}

	m_render.BeginSceneDev();
	// render things
	m_render.EndScene();

	return false;
}
