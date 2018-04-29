#include <qapplication.h>
#include <qthread.h>

#include "render/renderer.h"
#include "utils/ResourceManager.h"
#include "utils/ResourcePreloader.h"
#include "utils/InitializeSerializer.h"

#include "ViewModule.h"

#include "utils/Event.h"

class QCloseEvent;

namespace Grafkit {
	class IAssetFactory;
}

#define APP_NAME "Idogep"

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
		static EditorApplication* Instance() { return s_self; }

	public:
		EditorApplication(int argc, char **argv);
		~EditorApplication();

		int execute();

		//LoggerQTProxy* GetLoggerProxy() { return m_logger; }

		// --- 
		private slots:
		void mainloop();
		void loaderFinished();
		void focusChanged(QWidget*from, QWidget* to) { onFocusChanged(from, to); }

	private:
		virtual Grafkit::Renderer &GetDeviceContext() { return m_render; }
		virtual Grafkit::IAssetFactory *GetAssetFactory() { return m_assetFactory; }

	private:
		void nextTick();
		void preload();

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

	class LoaderThread : public QThread
	{
		Q_OBJECT
	public:
		LoaderThread(QObject* parent = NULL);
		void run();
	};
}
