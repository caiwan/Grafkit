#include <QApplication>
#include <QTimer>
#include <QFile>

#include "main_editor.h"

#include "Application.h"
#include "proxies/LoggerProxy.h"

#include "mainWindow.h"
#include "modules/splashScreen/splashwidget.h"
#include "modules/preloaderDialog/preloader.h"
#include "modules/grafkitContext/QGrafkitContextWidget.h"

using namespace Idogep;

EditorApplicationQt::EditorApplicationQt(int argc, char** argv) : EditorApplication(argc, argv), m_qApp(argc, argv)
{
	QCoreApplication::setOrganizationName("IndustrialRevolutioners");
	QCoreApplication::setOrganizationDomain("caiwan.github.io");
	QCoreApplication::setApplicationName(APP_NAME);

	QFile f(":/css/global.css"); f.open(QFile::ReadOnly);
	m_qApp.setStyleSheet(f.readAll()); f.close();
}

EditorApplicationQt::~EditorApplicationQt()
{
    //... 
}

int EditorApplicationQt::ExecuteParentFramework()
{
	m_editor = new Editor(m_render, this);
	m_mainWindow = new MainWindow(m_editor);

	m_preloadWindow = new Preloader(m_mainWindow);
	onFocusChanged += Delegate(m_preloadWindow, &Preloader::FocusChanged);

	SetPreloadListener(m_preloadWindow);

	SplashWidget *sw = new SplashWidget();

	LoaderThread *loader = new LoaderThread();

	onLoaderFinished += Delegate(sw, &SplashWidget::hide);
	onLoaderFinished += Delegate(sw, &SplashWidget::deleteLater);
	onLoaderFinished += Delegate(m_mainWindow, &MainWindow::showMaximized);
	onLoaderFinished += Delegate(this, &EditorApplication::nextTick);
	onLoaderFinished += Delegate(loader, &LoaderThread::deleteLater);

	sw->show();

	BuildMainWindow();
	BuildEditorModules();

	connect(loader, SIGNAL(finished()), this, SLOT(loaderFinished()));
	loader->start();

	m_renderWidget = new QGrafkitContextWidget(m_render);
	m_renderWidget->Initialize();

	m_mainWindow->setCentralWidget(m_renderWidget);

	m_editor->NewDocument();

	return m_qApp.exec();
}

void EditorApplicationQt::StartLoaderThread()
{
    auto *loader = new LoaderThread();
	onLoaderFinished += Delegate(loader, &LoaderThread::deleteLater);
	connect(loader, SIGNAL(finished()), this, SLOT(loaderFinishedSlot()));
	loader->start();
}

void EditorApplicationQt::NextTick()
{
	QTimer::singleShot(0, this, SLOT(mainloopSlot()));
}

// ========================================================================================

int main(int argc, char **argv)
{
	Q_INIT_RESOURCE(resources);

	qInstallMessageHandler(myMessageOutput);

	auto app = new EditorApplicationQt(argc, argv);
	return app->Execute();
}

