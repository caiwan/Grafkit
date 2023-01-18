#include <QApplication>

#include <QDebug>

#include "context.h"

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

EditorApplication* EditorApplication::s_self;

EditorApplication::EditorApplication(int argc, char** argv) : IResourceManager()
    , ClonableInitializer()
    , m_assetFactory(nullptr)
    , m_demoContext(nullptr)
    , m_preloadWindow(nullptr)
    , m_editor(nullptr)
{
    assert(s_self == nullptr);

    // init the rest of the things 
    m_logger = new LoggerProxy();
    Log::Logger().AddHandler(m_logger);

    // ... 
    // Argparse goez here if needed 

    m_projectFileLoader = new FileAssetFactory("./");
    m_assetFactory = new AssetFactoryProxy(m_projectFileLoader);

    s_self = this;
}


EditorApplication::~EditorApplication()
{
    m_render.Shutdown();
    delete m_assetFactory;
    delete m_projectFileLoader;
    delete m_demoContext;
}


int EditorApplication::Execute()
{
    m_mainWindow = new MainWindow();
    this->Add(new Resource<View>(m_mainWindow, "EditorView", "052de2ba-ae61-4b60-a723-f1259ffd5a32"));

    SplashWidget* sw = new SplashWidget();

    onLoaderFinished += Delegate(sw, &SplashWidget::hide);
    onLoaderFinished += Delegate(sw, &SplashWidget::deleteLater);
    onLoaderFinished += Delegate(m_mainWindow, &MainWindow::showMaximized);
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
    m_demoContext = new GkDemo::Context(m_render, m_assetFactory);
    m_preloadWindow = new Preloader(m_mainWindow);
    onFocusChanged += Delegate(m_preloadWindow, &Preloader::FocusChanged);

    m_demoContext->SetPreloadListener(m_preloadWindow);

    // --- Setup modules

    m_editor = new Editor(m_render, m_demoContext);
    this->Add(new Resource<Controller>(m_editor, "Editor", "bcfdd3ee-29cc-455a-a76f-d06fda684dbc"));

    m_logModule = new LogModule(m_logger);
    this->Add(new Resource<Controller>(m_logModule, "LogModule", "8d23d4f2-a040-4a4e-bc0f-9d9a37977e5b"));
    this->Add(new Resource<View>(new LogWidget(), "LogView", "5d8e4ef7-5cc6-424f-88c6-063840db1c5b"));

    // --- 
    m_outlineViewModule = new OutlineModule();
    this->Add(new Resource<Controller>(m_outlineViewModule, "OutlineModule", "585d0fc1-6cf8-435f-aa38-777e4db40f1d"));
    this->Add(new Resource<View>(new SceneGraphViewWidget(), "OutlineView", "5fbe4ef8-91ca-4cab-99a8-8e811318bcfb"));

    m_editor->onDocumentChanged += Delegate(dynamic_cast<OutlineModule*>(m_outlineViewModule.Get()), &OutlineModule::DocumentChangedEvent);

    // --- 
    m_animationEditor = new AnimationEditor();
    this->Add(new Resource<Controller>(m_animationEditor, "AnimationEditor", "6de072f7-50d4-4b14-82fd-337ccb758f0a"));
    this->Add(new Resource<View>(new AnimationEditorWidget, "AnimationView", "e1b0d54a-5f5f-4108-8bf8-1927c8d88d66"));

    this->Add(new Resource<Controller>(new CurveEditor(), "CurveEditor", "e6da51b2-689e-4c18-a889-8ddb2f62ee69"));
    this->Add(new Resource<View>(new CurveEditorScene(), "CurveEditorView", "71a94299-4151-48ff-97df-f96504a7b4c4"));

    this->Add(new Resource<Controller>(new CurvePointEditor(), "CurvePointEditor", "c54a9d93-91cc-4599-bac4-cfcc199daf1b"));
    this->Add(new Resource<View>(new PointEditorWidget(), "PointEditorView", "0aad5739-4959-4e6d-bdeb-abadf3de524d"));
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
    //QDockWidget* curveEditorWidget = dynamic_cast<QDockWidget*>(m_animationEditor->GetView().Get());
    //assert(curveEditorWidget);

    //m_mainWindow->addDockWidget(Qt::BottomDockWidgetArea, curveEditorWidget);

    // --- 
    QDockWidget* logWidget = dynamic_cast<QDockWidget*>(View::SafeGetView(this, "LogView").Get());
    assert(logWidget);

    logWidget->setParent(m_mainWindow);
    m_mainWindow->addDockWidget(Qt::BottomDockWidgetArea, logWidget);
    //m_mainWindow->tabifyDockWidget(logWidget, curveEditorWidget);

    // -- 
    QDockWidget* outlineWidget  = dynamic_cast<QDockWidget*>(View::SafeGetView(this, "OutlineView").Get());
    assert(outlineWidget);

    outlineWidget->setParent(m_mainWindow);
    m_mainWindow->addDockWidget(Qt::LeftDockWidgetArea, outlineWidget);

    //// connect redo / undo menus
    //m_editor->GetCommandStack()->onCommandStackChanged += Delegate(m_mainWindow, &Roles::ManageCommandStackRole::CommandStackChangedEvent);
    //m_mainWindow->onUndo += Delegate(m_editor->GetCommandStack(), &CommandStack::Undo);
    //m_mainWindow->onRedo += Delegate(m_editor->GetCommandStack(), &CommandStack::Redo);
}


// ========================================================================================

LoaderThread::LoaderThread(QObject* parent) : QThread(parent) {
}

void LoaderThread::run() { msleep(1000); }
