#pragma once

#include <string>

#include "common.h"

#include "utils/Event.h"
#include "utils/Command.h"

#include "utils/ViewModule.h"

#include "EditorView.h"

namespace Grafkit {
	class IResourceManager;
}

namespace Idogep {

	class Editor;
	class EditorApplication;

	class Document;

	class MusicProxy;

	class Editor : public Module{
	public:
		Editor(Ref<Module> parent, Grafkit::Renderer &render, Grafkit::IResourceManager * const & resman);
		~Editor();

		void Initialize() override;
		//void MediateSiblingModule(Ref<Module> module);

		void InitializeDocument();

		bool RenderFrame();
		
		//void RequestLoad();
		//void RequestReload();

		void NewDocument();
		void SaveDocument();
		void OpenDocument(std::string filename);

		//void ExitingApp(bool &isPreventExit);

		// ... 

		// Events
		//Event<std::string&> onSaveDialog;
		Event<Document* const &> onDocumentChanged;

		Grafkit::IResourceManager *GetResourceManager();

		Document * GetDocument() { return m_document; }
		CommandStack * GetCommandStack() { return m_commandStack; }
		MusicProxy * GetMusicProxy() { return m_musicProxy; }
		
	private:
		bool DirtyCheck();
		
	private:
		Grafkit::IResourceManager * const & m_resourceManager;
		Grafkit::Renderer &m_render;

		MusicProxy * m_musicProxy;

		CommandStack* m_commandStack;
		Document * m_document;
		
		bool m_precalcRequested;
		bool m_reloadRequested;

	};

}

