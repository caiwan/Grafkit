#include "CurveModule.h"

#include "utils/tree/treeitem.h"
#include "utils/tree/treemodel.h"

#include "curveeditorwidget.h"
#include "CurveView.h"

#include <QWidget>

using namespace Idogep;
using namespace Grafkit;

Idogep::CurveEditorModule::CurveEditorModule(Ref<Module> parent) : Module(parent)
{
}

Idogep::CurveEditorModule::~CurveEditorModule()
{
}

void Idogep::CurveEditorModule::Initialize()
{
	assert(m_parent);
	assert(m_parent->GetView());

	QWidget* parentWidget = dynamic_cast<QWidget *>(m_parent->GetView().Get());
	assert(parentWidget);
	m_myView = new CurveEditorWidget(parentWidget);
	SetView(m_myView);
}
