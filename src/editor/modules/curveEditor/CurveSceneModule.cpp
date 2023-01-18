#include "CurveSceneModule.h"
#include "CurveSceneView.h"

// impl
#include "curveeditorwidget.h"
#include "curveeditorscene.h"

using namespace Idogep;

CurveSceneModule::CurveSceneModule(Ref<Module> parent) : Module(parent)
{
}

CurveSceneModule::~CurveSceneModule()
{
}

void CurveSceneModule::Initialize()
{
	assert(m_parent);
	assert(m_parent->GetView());

    CurveEditorWidget* parentWidget = dynamic_cast<CurveEditorWidget* >(m_parent->GetView().Get());
	assert(parentWidget);

	CurveEditorScene *ces = new CurveEditorScene();

	parentWidget->SetGraphicsScene(ces);

    // 
	m_myView = ces; SetView(m_myView);
}
