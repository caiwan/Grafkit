#pragma once

#include <qapplication.h>
#include <qthread.h>

#include "render/renderer.h"
#include "utils/ResourcePreloader.h"
#include "utils/InitializeSerializer.h"

#include "utils/Event.h"

#define APP_NAME "Idogep"

namespace Grafkit {
	class IAssetFactory;
}

namespace GkDemo
{
    class Context;
    class Demo;
}

namespace Idogep {

	//class Document;
	class Editor;

	class MainWindow;
	class Preloader;

	class QGrafkitContextWidget;

	class LoggerProxy;

	class Controller;
	class Playback;

	class EditorApplication :
		public Grafkit::IResourceManager, Grafkit::ClonableInitializer
	{
	public:
		static EditorApplication* Instance() { return s_self; }

		// --- 
		EditorApplication(int argc, char **argv);
		~EditorApplication();

	    int Execute();

	private:
		Grafkit::Renderer &GetDeviceContext() override { return m_render; }
		Grafkit::IAssetFactory *GetAssetFactory() override { return m_assetFactory; }

		virtual int ExecuteParentFramework() = 0;

	protected:
		// Playback role
		void Mainloop();
		virtual void NextTick() = 0;

	private:

		void Preload();
		virtual void StartLoaderThread() = 0;

		void Initialize();

		void BuildEditorModules();
		void InitializeModules();
		void BuildDockingWindows();

	public:
		Event<> onLoaderFinished;
		Event<QWidget* const&, QWidget* const&> onFocusChanged;

	private:
		Grafkit::Renderer m_render;
		Grafkit::IAssetFactory *m_assetFactory;
		Grafkit::IAssetFactory *m_projectFileLoader;

        GkDemo::Context *m_demoContext;

		QGrafkitContextWidget *m_renderWidget;

		MainWindow *m_mainWindow{};
		Preloader *m_preloadWindow;

		Ref<Editor> m_editor;

		// --- modules
		LoggerProxy * m_logger = nullptr;

		Ref<Controller> m_logModule;

		Ref<Controller> m_outlineViewModule;
		Ref<Controller> m_animationEditor;

		// --- singleton element
		static EditorApplication * s_self;
	};

	// --- 
	// later on, as the app grows the entire application initialization process should be put here or so

	class LoaderThread : public QThread
	{
		Q_OBJECT
	public:
	    explicit LoaderThread(QObject* parent = nullptr);
		void run() override;
	};

}
