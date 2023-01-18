#include "outlineModule.h"

#include <QWidget>

#include "models/Document.h"

#include "scenegraphviewwidget.h"

using namespace Idogep;
using namespace Grafkit;

Idogep::OutlineView::OutlineView() 
{
}


Idogep::OutlineModule::OutlineModule(Ref<Module> parent) : Module(parent), m_myView(nullptr), m_myModel(nullptr)
{
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
    // build shit 

    // ... 

    // replace shit 
	delete m_myModel;
	m_myModel = newModel;

    // push
	const auto model = new Resource<SceneGraphViewWidgetModel>(m_myModel);
	m_myView->SetModel(model);
}

