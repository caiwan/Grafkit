#pragma once

#include <string>

#include "render/renderer.h"

#include "utils/Event.h"
#include "utils/Command.h"

namespace Grafkit {
	class IResourceManager;
}

namespace Idogep {

	class Editor;
	class EditorApplication;

	class Document;

	//class MusicProxy;
	//class LoggerProxy;

	class TreeModel;
	class SceneGraphModel;

	class Editor {
	public:
		Editor(Grafkit::Renderer &render, Grafkit::IResourceManager * const & resman);
		~Editor();

		void InitializeDocument();

		bool RenderFrame();
		
		void RequestLoad();
		void RequestReload();

		void NewDocument();
		void SaveDocument();
		void OpenDocument(std::string filename);

		void ExitingApp(bool &isPreventExit);

		// ... 

		// Events
		Event<std::string&> onSaveDialog;
		Event<Document* const &> onDocumentChanged;

		// ... 

		Grafkit::IResourceManager *GetResourceManager();

		Document * const & GetDocument() { return m_document; }
		CommandStack * GetCommandStack() { return m_commandStack; }
		
	private:
		bool DirtyCheck();
		
	private:
		Grafkit::IResourceManager * const & m_resourceManager;
		Grafkit::Renderer &m_render;

		CommandStack* m_commandStack;
		Document * m_document;
		//MusicProxy *m_musicProxy;
		
		bool m_precalcRequested;
		bool m_reloadRequested;

	};

}

