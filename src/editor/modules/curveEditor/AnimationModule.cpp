#include "AnimationModule.h"

#include "utils/tree/treeitem.h"
#include "utils/tree/treemodel.h"

#include "CurveEditor.h"

#include "Editor.h"
#include "proxies/MusicProxy.h"

#include "models/OutlineItems.h"
#include "AnimationEditorWidget.h"
#include "AnimationTreeModel.h"

using namespace Idogep;
using namespace Grafkit;

// ===========================================================================================

AnimationEditorModule::AnimationEditorModule(const Ref<Controller>& parent)
    : Controller(parent) {
    m_curveScene = new CurveEditor(this);
}

AnimationEditorModule::~AnimationEditorModule()
= default;

void AnimationEditorModule::Initialize()
{
    assert(m_parent);
    assert(m_parent->GetView());

    QWidget* parentWidget = dynamic_cast<QWidget *>(m_parent->GetView().Get());
    assert(parentWidget);
    m_myView = new AnimationEditorWidget(parentWidget);

    // manage playback role 
    Editor* editor = dynamic_cast<Editor*>(GetRootModule().Get());
    assert(editor);

    Timer* timer = editor->GetMusicProxy();
    assert(timer);

    m_myView->onTogglePlayback += Delegate(timer, &Timer::TogglePlay);
    m_myView->onStopPlayback += Delegate(timer, &Timer::Stop);
    // ... 

    // manage animation role
    m_myView->onChannelSelected += Delegate(m_curveScene.Get(), &CurveEditor::ChannelSelectedEvent);

    SetView(m_myView);
}


// ========================================================================================================

//Ref<CurveEditor> AnimationEditorModule::GetCurveSceneMoule() const
//{
//}

void AnimationEditorModule::AnimationSelectedEvent(TreeItem* const item)
{
    ItemHasAnimationsRole* animationItem = dynamic_cast<ItemHasAnimationsRole*>(item);
    if (!animationItem) // hide elements
    {
        m_myView->onChannelSelected(nullptr);
        m_myView->UpdateAnimationModel(nullptr);
        return;
    }

    AnimationWrapperRef animation = animationItem->GetAnimation();

    if (!animation->GetAnimationListModel()) // TODO -> Animationwrapper maybe?
    {
        AnimationTreeModel* newModel = new AnimationTreeModel(animation);
        newModel->BuildModel();
        animation->SetAnimationListModel(newModel);
    }

    m_animationListModel = animation->GetAnimationListModel();
    m_myView->UpdateAnimationModel(m_animationListModel);
}
