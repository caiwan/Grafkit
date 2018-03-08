#pragma once

#include "render/renderer.h"

#include "Event.h"
#include "Command.h"

namespace Grafkit {
	class IResourceManager;
}

namespace Idogep {

	class Editor;
	class EditorApplication;

	class Document;

	class MusicProxy;

	class Editor {

	public:
		Editor(Grafkit::Renderer &render, Grafkit::IResourceManager * const & resman);
		~Editor();

		void Initialize();

		bool RenderFrame();
		
		void RequestLoad();
		void RequestReload();

		void NewDocument();
		void SaveDocument(std::string filename);
		void OpenDocument(std::string filename);

		Grafkit::IResourceManager *GetResourceManager() { return m_resourceManager; }

		Document * Getdocument() { return m_document; }
		CommandStack * GetCommandStack() { return m_commandStack; }
		MusicProxy * GetMusicProxy() { return m_musicProxy; }

		// Events
		Event<const Document * const &> onDocumentChanged;
		
	private:
		Grafkit::IResourceManager * const & m_resourceManager;
		Grafkit::Renderer &m_render;

		CommandStack* m_commandStack;
		Document * m_document;
		MusicProxy *m_musicProxy;

		bool m_precalcRequested;
		bool m_reloadRequested;

	};

}

