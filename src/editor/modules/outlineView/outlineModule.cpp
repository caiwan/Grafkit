#include "outlineModule.h"

#include <qwidget.h>

#include "utils/resource.h"

#include "models/OutlineItems.h"
#include "models/Document.h"

#include "scenegraphviewwidget.h"

using namespace Idogep;
using namespace Grafkit;

Idogep::OutlineView::OutlineView()
{
}


Idogep::OutlineModule::OutlineModule(Ref<Module> parent) : Module(parent)
{
}

void Idogep::OutlineModule::Initialize()
{
	assert(m_parent.Valid());
	assert(m_parent->GetView().Valid());
	QWidget * parentWidget = dynamic_cast<QWidget*>(m_parent->GetView().Get());
	assert(parentWidget);

	m_myView = new SceneGraphViewWidget(parentWidget);
		 
	SetView(m_myView);

}

void Idogep::OutlineModule::MediateSiblingModule(Ref<Module> other)
{
	RecievesOutlineTreechangeRole *reciever = dynamic_cast<RecievesOutlineTreechangeRole *>(other.Get());
	if (reciever) {

	} else {
		assert(false);
	}

}

