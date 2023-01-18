#include "outlineModule.h"

#include "demo.h"

#include "outlineTreeModel.h"

#include "scenegraphviewwidget.h"

using namespace Idogep;
using namespace Grafkit;


// -------------------------------------------------------------------------------------------------------------------

OutlineModule::OutlineModule() : Controller()
    , m_myModel(nullptr)
    , m_modelBuilder(nullptr) 
{
}

OutlineModule::~OutlineModule()
{
    delete m_modelBuilder;
}

void OutlineModule::Initialize(IResourceManager* const& resourceManager)
{
    m_myView = dynamic_cast<OutlineView*>(View::SafeGetView(resourceManager, "OutlineView").Get());
    assert(m_myView.Valid());

    m_myView->m_outlineModule = this; // we sould not do this
}

void OutlineModule::DocumentChangedEvent(GkDemo::Demo* const& demo)
{
    assert(m_myView);
    assert(demo);

    auto newModel = new SceneGraphViewWidgetModel();

    // build stuff
    const auto scenegraph = demo->GetScene();
    const auto newModelBuilder = new SceneModel(scenegraph);
    newModelBuilder->BuildModel();
    newModel->SetModel(newModelBuilder);

    // replace shit 
    delete m_myModel;
    m_myModel = newModel;

    // push
    m_myView->SetModel(m_myModel);
}

// --- 

OutlineView::OutlineView() : View() {
}
