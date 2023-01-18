#include "CurveModule.h"

#include "utils/tree/treeitem.h"
#include "utils/tree/treemodel.h"

#include "CurveView.h"
#include "curveeditorwidget.h"

#include "CurveSceneModule.h"

#include "Editor.h"
#include "proxies/MusicProxy.h"

#include <QWidget>
#include "models/OutlineItems.h"

using namespace Idogep;
using namespace Grafkit;

// ===========================================================================================

CurveEditorModule::CurveEditorModule(const Ref<Module>& parent)
	: Module(parent)
{
	m_curveScene = new CurveSceneModule(this);
}

CurveEditorModule::~CurveEditorModule()
= default;

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

    // manage animation role
	m_myView->onChannelSelected += Delegate(m_curveScene.Get(), &CurveSceneModule::ChannelSelectedEvent);

	SetView(m_myView);
}


// ========================================================================================================

//Ref<CurveSceneModule> CurveEditorModule::GetCurveSceneMoule() const
//{
//}

void Idogep::CurveEditorModule::AnimationSelectedEvent(TreeItem * const item)
{
	ItemHasAnimationsRole* animationItem = dynamic_cast<ItemHasAnimationsRole*>(item);
	if (!animationItem)
	{
		return; // clean widget instead
	};
	m_myView->AnimationChangedEvent(animationItem->GetAnimation(),
		animationItem->GetParentName());
}

