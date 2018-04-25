#include <qapplication.h>

#include <qtimer.h>

#include <qmessagebox.h>
#include <qevent.h>

#include "common.h"
//#include "utils/"

#include "utils/AssetFile.h"

#include "main_editor.h"
#include "editor.h"

#include "proxies/AssetFactoryProxy.h"
#include "proxies/LoggerProxy.h"

#include "mainWindow.h"
#include "modules/splashScreen/splashwidget.h"
#include "modules/preloaderDialog/preloader.h"
#include "modules/grafkitContext/QGrafkitContextWidget.h"

using namespace Idogep;
using namespace Grafkit;

Idogep::EditorApplication * Idogep::EditorApplication::s_self;

Idogep::EditorApplication::EditorApplication(int argc, char **argv) :
	QObject(nullptr),
	Grafkit::ClonableInitializer(),
	Grafkit::ResourcePreloader(),
	m_qApp(argc, argv), m_editor(nullptr), m_logger(nullptr), m_preloadWindow(nullptr)
{
	assert(s_self == nullptr);

	// connect signals and slots

	// init the rest of the things 
	m_logger = new LoggerQTProxy();
	Grafkit::Log::Logger().AddHandler(m_logger);

	QCoreApplication::setOrganizationName("IndustrialRevolutioners");
	QCoreApplication::setOrganizationDomain("caiwan.github.io");
	QCoreApplication::setApplicationName(APP_NAME);

	QFile f(":/css/global.css"); f.open(QFile::ReadOnly);
	m_qApp.setStyleSheet(f.readAll()); f.close();

	// ... 
	// Argparse goez here if needed 

	m_projectFileLoader = new FileAssetFactory("./");
	m_assetFactory = new AssetFactoryProxy(m_projectFileLoader);

	s_self = this;
}


Idogep::EditorApplication::~EditorApplication()
{
	m_render.Shutdown();
	delete m_assetFactory;
	delete m_projectFileLoader;
}


int Idogep::EditorApplication::execute()
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

void Idogep::EditorApplication::mainloop()
{
	bool renderNextFrame = this->m_editor->RenderFrame();
	if (renderNextFrame)
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

void Idogep::EditorApplication::preload()
{
	DoPrecalc();
	m_editor->InitializeDocument();
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
	
	qInstallMessageHandler(Idogep::myMessageOutput);

	auto app = new Idogep::EditorApplication(argc, argv);
	return app->execute();
}
