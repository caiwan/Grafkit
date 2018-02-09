#include <qapplication.h>
#include <qpushbutton.h>

#include "QGrafkitContextWidget.h"

int main( int argc, char **argv )
{
	Q_INIT_RESOURCE(resources);

    QApplication a( argc, argv );

	Grafkit::Renderer renderer;
	QGrafkitContextWidget *wnd = new QGrafkitContextWidget(renderer);

	wnd->setFixedSize(800, 600);
	wnd->initialize();

	wnd->show();

    return a.exec();
}
