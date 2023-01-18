#include <qapplication.h>

#include <qtimer.h>

#include <qmessagebox.h>
#include <qevent.h>

#include <qdockwidget.h>

#include <stack>

#include "common.h"
//#include "utils/"

#include "utils/AssetFile.h"

#include "main_editor.h"

#include "Application.h"
#include "proxies/LoggerProxy.h"

#include "mainWindow.h"
#include "modules/splashScreen/splashwidget.h"
#include "modules/preloaderDialog/preloader.h"
#include "modules/grafkitContext/QGrafkitContextWidget.h"

#include "modules/logView/LogModule.h"
#include "modules/outlineView/outlineModule.h"

using namespace Idogep;

EditorApplicationQt::EditorApplicationQt(int argc, char** argv) : EditorApplication(argc, argv), m_qApp(argc, argv)
{
	assert(s_self == nullptr);

	// connect signals and slots

	// init the rest of the things 
	m_logger = new LoggerProxy();
	Grafkit::Log::Logger().AddHandler(m_logger);

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
	m_editor = new Editor(nullptr, m_render, this);
	m_mainWindow = new MainWindow();
	m_editor->SetView(m_mainWindow);

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

	// this one should put out to a plugin system
	BuildEditorModules();
	InitializeModules();
	BuildDockingWindows();

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
// This part should be refactored evetnutally:

void Idogep::EditorApplication::BuildEditorModules()
{
	m_logModule = new LogModule(m_editor, m_logger);
	m_outlineViewModule = new OutlineModule(m_editor);
}

void Idogep::EditorApplication::InitializeModules() {
	std::stack<Ref<Module>> stack;
	stack.push(m_editor);
	while (!stack.empty()) {
		Ref<Module> module = stack.top(); stack.pop();

		module->Initialize();

		for (size_t i = 0; i < module->GetChildModuleCount(); i++) {
			stack.push(module->GetChildModule(i));
		}
	}
}

void Idogep::EditorApplication::BuildDockingWindows()
{
	QDockWidget *logWidget = dynamic_cast<QDockWidget*>(m_logModule->GetView().Get());
	assert(logWidget);

	logWidget->setParent(m_mainWindow);
	m_mainWindow->addDockWidget(Qt::BottomDockWidgetArea, logWidget);
	//m_mainWindow->tabifyDockWidget(m_logWidget, m_curveEditor);

	// 
	QDockWidget *outlineWidget = dynamic_cast<QDockWidget*>(m_outlineViewModule->GetView().Get());
	assert(outlineWidget);

	outlineWidget->setParent(m_mainWindow);
	m_mainWindow->addDockWidget(Qt::LeftDockWidgetArea, outlineWidget);
}


// ========================================================================================

Idogep::LoaderThread::LoaderThread(QObject* parent) : QThread(parent)
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

