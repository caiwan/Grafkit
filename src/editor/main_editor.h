#include "qapplication.h"
#include "render/renderer.h"

namespace Idogep {

	class MainWindow;
	class QGrafkitContextWidget;

	class EditorApplication : QObject {
		Q_OBJECT
	public:
		EditorApplication(int argc, char **argv);

		int execute();

	private slots:
		void mainloop();

	private:
		void nextTick();

	private:
		QApplication m_qApp;
		Grafkit::Renderer m_render;

		Idogep::QGrafkitContextWidget *m_widget;
		Idogep::MainWindow *m_wnd;
	};
}
