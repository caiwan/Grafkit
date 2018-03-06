#include <qapplication.h>
#include <qthread.h>

#include "render/renderer.h"
#include "utils/ResourceManager.h"
#include "utils/InitializeSerializer.h"

#include "Event.h"

class QCloseEvent;

namespace Grafkit{
	class IAssetFactory;
}

#define APP_NAME "Idogep"

namespace Idogep {

	class EditorDocument;

	class MainWindow;
	class QGrafkitContextWidget;

	class EditorApplication : public QObject ,
		protected Grafkit::IResourceManager, private Grafkit::ClonableInitializer
	{
		Q_OBJECT

	public:
		static EditorApplication* Instance() { return s_self; }

	public:
		EditorApplication(int argc, char **argv);

		int execute();

		void onMainWindowClose(QCloseEvent *event);

		void onNew();
		//void onSave();
		//void onOpen();

		private slots:
		void mainloop();
		void loaderFinished();

		//void playbackOptions();

	private:
		virtual Grafkit::Renderer &GetDeviceContext() { return m_render; }
		virtual Grafkit::IAssetFactory *GetAssetFactory() { return m_file_loader; }

	private:
		void nextTick();
		void preload();

	protected:

	private:
		Event<> onLoaderFinished;
		Event<Grafkit::IResourceManager* const &> onBeforePreload;
		Event<Grafkit::Renderer&> onAfterPreload;

		QApplication m_qApp;
		Grafkit::Renderer m_render;
		Grafkit::IAssetFactory *m_file_loader;

		Idogep::QGrafkitContextWidget *m_widget;
		Idogep::MainWindow *m_wnd;
		Idogep::EditorDocument * m_document;

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
