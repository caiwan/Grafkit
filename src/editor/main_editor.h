#include <qapplication.h>
#include <qthread.h>

#include "render/renderer.h"
#include "utils/ResourceManager.h"
#include "utils/ResourcePreloader.h"
#include "utils/InitializeSerializer.h"

#include "Event.h"

class QCloseEvent;

namespace Grafkit{
	class IAssetFactory;
}

#define APP_NAME "Idogep"

namespace Idogep {

	class Document;
	class Editor; 

	class MainWindow;
	class QGrafkitContextWidget;

	class LoggerQTAdapter;

	class EditorApplication : public QObject ,
		protected Grafkit::ResourcePreloader, private Grafkit::ClonableInitializer
	{
		Q_OBJECT

	public:
		static EditorApplication* Instance() { return s_self; }

	public:
		EditorApplication(int argc, char **argv);
		~EditorApplication();

		int execute();

		private slots:
		void mainloop();
		void loaderFinished();

	private:
		virtual Grafkit::Renderer &GetDeviceContext() { return m_render; }
		virtual Grafkit::IAssetFactory *GetAssetFactory() { return m_assetFactory; }

	private:
		void nextTick();
		void preload();

	protected:

	private:
		Event<> onLoaderFinished;

		QApplication m_qApp;
		Grafkit::Renderer m_render;
		Grafkit::IAssetFactory *m_assetFactory;
		Grafkit::IAssetFactory *m_projectFileLoader;

		Idogep::LoggerQTAdapter *m_logger;
		Idogep::QGrafkitContextWidget *m_renderWidget;
		Idogep::MainWindow *m_mainWindow;
		
		Idogep::Editor *m_editor;

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
