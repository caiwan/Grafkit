#include <qapplication.h>
#include <qpushbutton.h>

#include <qtimer.h>

#include "main_editor.h"
#include "mainWindow.h"

#include "ui/QGrafkitContextWidget.h"

Idogep::EditorApplication::EditorApplication(int argc, char **argv) : QObject(nullptr),
m_qApp(argc, argv)
{
}

int Idogep::EditorApplication::execute()
{

	m_wnd = new MainWindow(this);


	m_widget = new QGrafkitContextWidget(m_render);
	//m_widget->setFixedSize(800, 600);

	m_widget->initialize();

	// ...
	m_wnd->setCentralWidget(m_widget);

	//m_widget->show();

	m_wnd->show();

	this->nextTick();

	return m_qApp.exec();
}

void Idogep::EditorApplication::mainloop()
{
	m_render.BeginSceneDev();
	m_render.EndScene();

	this->nextTick();
}

void Idogep::EditorApplication::nextTick()
{
	QTimer::singleShot(0, this, SLOT(mainloop()));
}


// ========================================================================================

int main(int argc, char **argv)
{
	Q_INIT_RESOURCE(resources);

	auto app = new Idogep::EditorApplication(argc, argv);
	return app->execute();
}

