#pragma once

#include <qapplication.h>
#include <qthread.h>

#include "render/renderer.h"
#include "utils/ResourceManager.h"
#include "utils/ResourcePreloader.h"
#include "utils/InitializeSerializer.h"

#include "ViewModule.h"

#include "utils/Event.h"

class QCloseEvent;

namespace Idogep {

	class Document;
	class Editor;

	class MainWindow;
	class Preloader;

	class QGrafkitContextWidget;

	class LoggerProxy;
	class LogModule;

	class EditorApplication : public QObject,
		protected Grafkit::ResourcePreloader, private Grafkit::ClonableInitializer
	{
		Q_OBJECT

	public:
		EditorApplicationQt(int argc, char **argv);
		~EditorApplicationQt();

	public:
		EditorApplication(int argc, char **argv);
		~EditorApplication();

		int execute();

		//LoggerQTProxy* GetLoggerProxy() { return m_logger; }

		// --- 
		private slots:
	    // ReSharper disable CppInconsistentNaming
		void loaderFinishedSlot() { onLoaderFinished(); }
		void focusChangedSlot(QWidget*from, QWidget* to) { onFocusChanged(from, to); }
		void mainloopSlot() { Mainloop(); }
	    // ReSharper restore CppInconsistentNaming

	private:
	    int ExecuteParentFramework() override;
	    void NextTick() override;

		void Initialize();

		void BuildEditorModules();
		void InitializeModules();
		void BuildDockingWindows();

	public:
		Event<> onLoaderFinished;
		Event<QWidget* const&, QWidget* const&> onFocusChanged;

	private:
		QApplication m_qApp;
		Grafkit::Renderer m_render;
		Grafkit::IAssetFactory *m_assetFactory;
		Grafkit::IAssetFactory *m_projectFileLoader;

		Idogep::QGrafkitContextWidget *m_renderWidget;

		Idogep::MainWindow *m_mainWindow;

		Idogep::Preloader *m_preloadWindow;

		Ref<Idogep::Editor> m_editor;

		// modules
		LoggerProxy *m_logger;
		Ref<Module> m_logModule;

		Ref<Module> m_outlineViewModule;

	private:
		static EditorApplication * s_self;
	};


}

