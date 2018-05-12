#include "CurveModule.h"

#include "utils/tree/treeitem.h"
#include "utils/tree/treemodel.h"

#include "CurveView.h"
#include "curveeditorwidget.h"

#include "CurveSceneModule.h"

#include "Editor.h"
#include "proxies/MusicProxy.h"

#include <QWidget>

using namespace Idogep;
using namespace Grafkit;

// ===========================================================================================

CurveEditorModule::CurveEditorModule(const Ref<Module>& parent)
	: Module(parent)
{
	m_curveScene = new CurveSceneModule(this);
}

CurveEditorModule::~CurveEditorModule()
{
}

void CurveEditorModule::Initialize()
{
	assert(m_parent);
	assert(m_parent->GetView());

	QWidget* parentWidget = dynamic_cast<QWidget *>(m_parent->GetView().Get());
	assert(parentWidget);
	m_myView = new CurveEditorWidget(parentWidget);

	// manage playback role 
	Editor* editor = dynamic_cast<Editor*>(GetRootModule().Get());
	assert(editor);

	Timer* timer = editor->GetMusicProxy();
	assert(timer);

	m_myView->onTogglePlayback += Delegate(timer, &Timer::TogglePlay);
	// ... 

	SetView(m_myView);
}

void Idogep::CurveEditorModule::AnimationSelectedEvent(TreeItem * const & item)
{
	// Kuki
}

