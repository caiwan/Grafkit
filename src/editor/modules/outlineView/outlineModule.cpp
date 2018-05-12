#include "outlineModule.h"

#include <QWidget>

#include "models/Document.h"

#include "outlineTreeModel.h"

#include "scenegraphviewwidget.h"

using namespace Idogep;
using namespace Grafkit;


Idogep::OutlineView::OutlineView()
{
}


// -------------------------------------------------------------------------------------------------------------------

Idogep::OutlineModule::OutlineModule(Ref<Module> parent) : Module(parent), m_myView(nullptr), m_myModel(nullptr), m_modelBuilder(nullptr)
{
}

Idogep::OutlineModule::~OutlineModule() {
	// delete m_myModel;
	delete m_modelBuilder;
}

void Idogep::OutlineModule::Initialize()
{
	assert(m_parent.Valid());
	assert(m_parent->GetView().Valid());
	const auto parentWidget = dynamic_cast<QWidget*>(m_parent->GetView().Get());
	assert(parentWidget);

	m_myView = new SceneGraphViewWidget(parentWidget);

	SetView(m_myView);
}

void OutlineModule::DocumentChangedEvent(Document* const& document)
{
	assert(m_myView);
	assert(document);

	auto newModel = new SceneGraphViewWidgetModel();

	/** 
	TODO ezt az egeszet ki kell innen vagni a francba majd, es valami ertelmessel kellene helyettesiteni
	*/

	// build shit 
	//const auto oldModelBuilder = m_myModel->GetModel();
	const auto scenegraph = document->GetScene();
	const auto newModelBuilder = new SceneModel(scenegraph);
	newModelBuilder->BuildModel();
	newModel->SetModel(newModelBuilder);

	// replace shit 
	delete m_myModel;
	//delete oldModelBuilder;
	m_myModel = newModel;

	// push
	const auto model = new Resource<SceneGraphViewWidgetModel>(m_myModel);
	m_myView->SetModel(model);
}

void Idogep::OutlineModule::ItemSelectedEvent(TreeItem  * const & item)
{
	auto *animation = dynamic_cast<ItemHasAnimationsRole*>(item);
	if (animation) {
		onAnimationItemSelected(item);
	}
}

