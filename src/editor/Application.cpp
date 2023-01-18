#include <QApplication>

#include <QDebug>

#include <stack>

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
	m_preloadWindow(nullptr), m_editor(nullptr)
{
	assert(s_self == nullptr);

	// init the rest of the things 
	m_logger = new LoggerProxy();
	Grafkit::Log::Logger().AddHandler(m_logger);

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



int Idogep::EditorApplication::Execute()
{
	m_editor = new Editor(nullptr, m_render, this);
	m_mainWindow = new MainWindow();
	m_editor->SetView(m_mainWindow);

	// --- 
	// TODO this part should be put out to somewhere later on: 

	m_preloadWindow = new Preloader(m_mainWindow);
	onFocusChanged += Delegate(m_preloadWindow, &Preloader::FocusChanged);

	SetPreloadListener(m_preloadWindow);

	SplashWidget *sw = new SplashWidget();

	onLoaderFinished += Delegate(sw, &SplashWidget::hide);
	onLoaderFinished += Delegate(sw, &SplashWidget::deleteLater);
	onLoaderFinished += Delegate(m_mainWindow, &MainWindow::showMaximized);
	onLoaderFinished += Delegate(this, &EditorApplication::NextTick);

	// --- 

	sw->show();

	// --- 

	Initialize();

	// qnd hack to wait a bit on splash screen
	StartLoaderThread();

	m_renderWidget = new QGrafkitContextWidget(m_render);
	m_renderWidget->Initialize();

	m_mainWindow->setCentralWidget(m_renderWidget);

	m_editor->NewDocument();

	return ExecuteParentFramework();
}

void EditorApplication::Mainloop()
{
	// ennek se feltetlenul itt lenne a helye
	const bool renderNextFrame = this->m_editor->RenderFrame();
	if (renderNextFrame)
		this->NextTick();
}

void Idogep::EditorApplication::Preload()
{
	DoPrecalc();
	m_editor->InitializeDocument();
}

// ========================================================================================

#include <QDockWidget>

#include "modules/logView/LogModule.h"
#include "modules/outlineView/outlineModule.h"
#include "modules/curveEditor/CurveModule.h"

// This part should be refactored eventutally:
// should put out to a plugin system

void EditorApplication::Initialize()
{
	BuildEditorModules();
	InitializeModules();
	BuildDockingWindows();
}

// TODO We shall put this out to a separate builder / mediator object
void Idogep::EditorApplication::BuildEditorModules()
{
	m_logModule = new LogModule(m_editor, m_logger);

	// --- 
	m_outlineViewModule = new OutlineModule(m_editor);

	m_editor->onDocumentChanged += Delegate(dynamic_cast<OutlineModule*>(m_outlineViewModule.Get()), &OutlineModule::DocumentChangedEvent);

	// --- 
	m_curveEditor = new CurveEditorModule(m_editor);

	// 
	dynamic_cast<OutlineModule*>(m_outlineViewModule.Get())->onItemSelected += Delegate(
		dynamic_cast<CurveEditorModule*>(m_curveEditor.Get()), &CurveEditorModule::AnimationSelectedEvent);
}

void Idogep::EditorApplication::InitializeModules() const
{
	std::stack<Ref<Module>> stack;
	stack.push(m_editor);
	while (!stack.empty()) {
		auto module = stack.top(); stack.pop();

		EmitsCommandRole* commandEmitter = dynamic_cast<EmitsCommandRole*>(module.Get());
		if (commandEmitter) {
			qDebug() << "Connecting command emitter obj=" << reinterpret_cast<void*>(module.Get());
			m_editor->GetCommandStack()->ConnectEmitter(commandEmitter);
		}

		module->Initialize();

		for (size_t i = 0; i < module->GetChildModuleCount(); i++) {
			stack.push(module->GetChildModule(i));
		}
	}
}

void Idogep::EditorApplication::BuildDockingWindows() const
{
	// 
	QDockWidget* curveEditorWidget = dynamic_cast<QDockWidget*>(m_curveEditor->GetView().Get());
	assert(curveEditorWidget);

	m_mainWindow->addDockWidget(Qt::BottomDockWidgetArea, curveEditorWidget);

	//
	QDockWidget* logWidget = dynamic_cast<QDockWidget*>(m_logModule->GetView().Get());
	assert(logWidget);

	logWidget->setParent(m_mainWindow);
	m_mainWindow->addDockWidget(Qt::BottomDockWidgetArea, logWidget);
	m_mainWindow->tabifyDockWidget(logWidget, curveEditorWidget);

	// 
	QDockWidget* outlineWidget = dynamic_cast<QDockWidget*>(m_outlineViewModule->GetView().Get());
	assert(outlineWidget);

	outlineWidget->setParent(m_mainWindow);
	m_mainWindow->addDockWidget(Qt::LeftDockWidgetArea, outlineWidget);

    // connect redo / undo menus
	m_editor->GetCommandStack()->onCommandStackChanged += Delegate(m_mainWindow, &Roles::ManageCommandStackRole::CommandStackChangedEvent);
	m_mainWindow->onUndo += Delegate(m_editor->GetCommandStack(), &CommandStack::Undo);
	m_mainWindow->onRedo += Delegate(m_editor->GetCommandStack(), &CommandStack::Redo);
}


// ========================================================================================

Idogep::LoaderThread::LoaderThread(QObject* parent) : QThread(parent)
{
}

void Idogep::LoaderThread::run()
{
	msleep(1000);
}

