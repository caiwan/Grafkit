#include <qapplication.h>

#include <qtimer.h>

#include <qmessagebox.h>
#include <qevent.h>

#include "utils/AssetFile.h"

#include "main_editor.h"

#include "document.h"

#include "ui/splashwidget.h"
#include "ui/mainWindow.h"
#include "ui/QGrafkitContextWidget.h"

using namespace Idogep;
using namespace Grafkit;

Idogep::EditorApplication::EditorApplication(int argc, char **argv) :
	QObject(nullptr),
	Grafkit::ClonableInitializer(),
	Grafkit::IResourceManager(),
	m_qApp(argc, argv), m_document(nullptr)
{
	QCoreApplication::setOrganizationName("IndustrialRevolutioners");
	QCoreApplication::setOrganizationDomain("caiwan.github.io");
	QCoreApplication::setApplicationName(APP_NAME);

	QFile f(":/res/css/global.css"); f.open(QFile::ReadOnly);
	m_qApp.setStyleSheet(f.readAll()); f.close();
	
	m_file_loader = new FileAssetFactory("./");
}

int Idogep::EditorApplication::execute()
{
	m_wnd = new MainWindow(this);

	SplashWidget *sw = new SplashWidget();

	LoaderThread *loader = new LoaderThread();

	onLoaderFinished += Delegate(sw, &SplashWidget::hide);
	onLoaderFinished += Delegate(sw, &SplashWidget::deleteLater);
	onLoaderFinished += Delegate(m_wnd, &MainWindow::showMaximized);
	onLoaderFinished += Delegate(this, &EditorApplication::nextTick);
	onLoaderFinished += Delegate(loader, &LoaderThread::deleteLater);

	sw->show();

	connect(loader, SIGNAL(finished()), this, SLOT(loaderFinished()));
	loader->start();

	m_widget = new QGrafkitContextWidget(m_render);
	m_widget->initialize();

	// init stuff here

	m_wnd->setCentralWidget(m_widget);

	onNew();

	return m_qApp.exec();
}

void Idogep::EditorApplication::onMainWindowClose(QCloseEvent * event)
{
	bool changes = true;
	QMessageBox::StandardButton resBtn = QMessageBox::question(m_wnd, APP_NAME,
		tr("Are you sure?\n"),
		QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
		QMessageBox::Yes);

	if (resBtn != QMessageBox::Yes) {
		event->ignore();
	}
	else {
		event->accept();
	}
}

void Idogep::EditorApplication::onNew()
{
	// ablak goez here 
	m_document = new EditorDocument();
	m_wnd->setDocument(m_document);

	m_document->preload(this);
	DoPrecalc();
	m_document->preloaded(this);
}

void Idogep::EditorApplication::mainloop()
{
	m_render.BeginSceneDev();
	m_render.EndScene();

	this->nextTick();
}

void Idogep::EditorApplication::loaderFinished()
{
	onLoaderFinished();
}

void Idogep::EditorApplication::nextTick()
{
	QTimer::singleShot(0, this, SLOT(mainloop()));
}


// ========================================================================================

Idogep::LoaderThread::LoaderThread(QObject* parent) : QThread(parent)
{
}

void Idogep::LoaderThread::run()
{
	msleep(1000);
}


// ========================================================================================

int main(int argc, char **argv)
{
	Q_INIT_RESOURCE(resources);

	auto app = new Idogep::EditorApplication(argc, argv);
	return app->execute();
}
