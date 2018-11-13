#include <QApplication>

#include <QDebug>

#include "context.h"

#include "utils/asset/AssetFile.h"

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

EditorApplication* EditorApplication::s_self;

EditorApplication::EditorApplication(int argc, char** argv) : IResourceManager()
    //, ClonableInitializer()
    , m_assetFactory(nullptr)
    , m_demoContext(nullptr)
    , m_preloadWindow(nullptr)
    , m_editor(nullptr)
{
    assert(s_self == nullptr);

    // init the rest of the things 
    m_logger = std::make_shared<LoggerProxy>();
    //m_logger = new ();
    Log::Logger().AddHandler(m_logger);

    // ... 
    // Argparse goez here if needed 

    m_projectFileLoader = std::make_shared<FileAssetFactory>("./");
    m_assetFactory = std::make_shared<AssetFactoryProxy>(m_projectFileLoader);

    s_self = this;
}


EditorApplication::~EditorApplication()
{
    m_render.Shutdown();
    //delete m_assetFactory;
    //delete m_projectFileLoader;
    //delete m_demoContext;
}


int EditorApplication::Execute()
{
    InitializeParentFramework();

    //m_mainWindow = new MainWindow();
    m_mainWindow = std::make_unique<MainWindow>();
    this->Add(new Resource<View>(m_mainWindow.get(), "EditorView", "EditorView"));

    SplashWidget* sw = new SplashWidget();

    // TODO: use std::functions and lambdas
    onLoaderFinished += Delegate(sw, &SplashWidget::hide);
    onLoaderFinished += Delegate(sw, &SplashWidget::deleteLater);
    onLoaderFinished += Delegate(m_mainWindow.get(), &MainWindow::showMaximized);
    onLoaderFinished += Delegate(this, &EditorApplication::NextTick);

    sw->show();

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

void EditorApplication::Preload()
{
    assert(0);
    DoPrecalc();
    m_editor->InitializeDocument();
}

// ========================================================================================

#include <QDockWidget>

#include "modules/logView/LogModule.h"
#include "modules/outlineView/outlineModule.h"
#include "modules/curveEditor/AnimationModule.h"
#include "modules/curveEditor/CurveEditor.h"
#include "modules/curveEditor/CurvePointEditor.h"

#include "modules/logView/logwidget.h"
#include "modules/outlineView/scenegraphviewwidget.h"
#include "modules/curveEditor/AnimationEditorWidget.h"
#include "modules/curveEditor/curveeditorscene.h"
#include "modules/curveEditor/PointEditorWidget.h"


// This part should be refactored eventutally:
// should put out to a plugin system

// TODO We shall put this out to a separate builder / mediator object
void EditorApplication::Initialize()
{
    BuildEditorModules();
    InitializeModules();
    BuildDockingWindows();
}

void EditorApplication::BuildEditorModules()
{
    // -- setup underlying demo context 
    m_demoContext = std::make_unique<GkDemo::Context>(m_render, m_assetFactory);
    m_preloadWindow = std::make_unique<Preloader>(m_mainWindow.get());
    onFocusChanged += Delegate(m_preloadWindow.get(), &Preloader::FocusChanged);

    m_demoContext->SetPreloadListener(m_preloadWindow.get());

    // --- Setup modules

    m_editor = new Editor(m_render, m_demoContext.get());
    this->Add(new Resource<Controller>(m_editor, "Editor", "Editor"));

    m_logModule = new LogModule(m_logger.get());
    this->Add(new Resource<Controller>(m_logModule, "LogModule", "LogModule"));
    this->Add(new Resource<View>(new LogWidget(), "LogView", "LogView"));

    // --- 
    m_outlineViewModule = new OutlineModule();
    this->Add(new Resource<Controller>(m_outlineViewModule, "OutlineModule", "OutlineModule"));
    this->Add(new Resource<View>(new SceneGraphViewWidget(), "OutlineView", "OutlineView"));

    m_editor->onDocumentChanged += Delegate(dynamic_cast<OutlineModule*>(m_outlineViewModule.Get()), &OutlineModule::DocumentChangedEvent);

    // --- 
    m_animationEditor = new AnimationEditor();
    this->Add(new Resource<Controller>(m_animationEditor, "AnimationEditor", "AnimationEditor"));
    this->Add(new Resource<View>(new AnimationEditorWidget, "AnimationView", "AnimationView"));

    this->Add(new Resource<Controller>(new CurveEditor(), "CurveEditor", "CurveEditor"));
    this->Add(new Resource<View>(new CurveEditorScene(), "CurveEditorView", "CurveEditorView"));

    this->Add(new Resource<Controller>(new CurvePointEditor(), "CurvePointEditor", "CurvePointEditor"));
    this->Add(new Resource<View>(new PointEditorWidget(), "PointEditorView", "PointEditorView"));
}

void EditorApplication::InitializeModules()
{
    std::list<Ref<IResource>> resources;
    GetAllResources(resources);
    for (auto resource : resources)
    {
        Ref<Resource<Controller>> controller = dynamic_cast<Resource<Controller>*>(resource.Get());
        if (controller.Valid() && controller->Valid())
        {
            qDebug() << "Initialize component" << QString::fromStdString(resource->GetName()) << "uuid=" << QString::fromStdString(resource->GetUuid());
            (*controller)->Initialize(this);

            EmitsCommandRole* commandEmitter = dynamic_cast<EmitsCommandRole*>(controller->Get());
            if (commandEmitter)
                m_editor->GetCommandStack()->ConnectEmitter(commandEmitter);
        }
    }
}

void EditorApplication::BuildDockingWindows()
{
    //// 
    AnimationEditorWidget* animationWidget = dynamic_cast<AnimationEditorWidget*>(View::SafeGetView(this, "AnimationView").Get());
    assert(animationWidget);

    CurveEditorScene *curveEditorScene= dynamic_cast<CurveEditorScene*>(View::SafeGetView(this, "CurveEditorView").Get());
    PointEditorWidget *pointEditorWidget = dynamic_cast<PointEditorWidget*>(View::SafeGetView(this, "PointEditorView").Get());

    animationWidget->SetGraphicsScene(curveEditorScene);
    animationWidget->SetPointEditorWidget(pointEditorWidget);

    m_mainWindow->addDockWidget(Qt::BottomDockWidgetArea, animationWidget);

    // --- 
    QDockWidget* logWidget = dynamic_cast<QDockWidget*>(View::SafeGetView(this, "LogView").Get());
    assert(logWidget);

    logWidget->setParent(m_mainWindow.get());
    m_mainWindow->addDockWidget(Qt::BottomDockWidgetArea, logWidget);
    m_mainWindow->tabifyDockWidget(logWidget, animationWidget);

    // -- 
    QDockWidget* outlineWidget  = dynamic_cast<QDockWidget*>(View::SafeGetView(this, "OutlineView").Get());
    assert(outlineWidget);

    outlineWidget->setParent(m_mainWindow.get());
    m_mainWindow->addDockWidget(Qt::LeftDockWidgetArea, outlineWidget);
}


// ========================================================================================

LoaderThread::LoaderThread(QObject* parent) : QThread(parent) {
}

void LoaderThread::run() { msleep(1000); }
