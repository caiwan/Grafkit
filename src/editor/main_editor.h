#include <qapplication.h>
#include <qthread.h>

#include "render/renderer.h"

#include "Event.h"

class QCloseEvent;

#define APP_NAME "Idogep"

namespace Idogep {

	class MainWindow;
	class QGrafkitContextWidget;

	class EditorApplication : QObject {
		Q_OBJECT
	public:
		EditorApplication(int argc, char **argv);

		int execute();

		void onMainWindowClose(QCloseEvent *event);

		private slots:
		void mainloop();
		void loaderFinished();

	private:
		void nextTick();

	private:
		Idogep::Event<> onLoaderFinished;

		QApplication m_qApp;
		Grafkit::Renderer m_render;

		Idogep::QGrafkitContextWidget *m_widget;
		Idogep::MainWindow *m_wnd;
	};

	class LoaderThread : public QThread
	{
		Q_OBJECT
	public:
		LoaderThread(QObject* parent = NULL);
		void run();
	};
}
