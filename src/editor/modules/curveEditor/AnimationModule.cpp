#include "AnimationModule.h"

#include "utils/tree/treeitem.h"
#include "utils/tree/treemodel.h"

#include "CurveEditor.h"

#include "Editor.h"
#include "proxies/MusicProxy.h"

#include "models/OutlineItems.h"
#include "modules/outlineView/outlineModule.h"
#include "AnimationEditorWidget.h"
#include "AnimationTreeModel.h"

using namespace Idogep;
using namespace Grafkit;


AnimationEditor::AnimationEditor()
    : Controller()
    , m_animationListModel(nullptr) {
}

AnimationEditor::~AnimationEditor()
= default;

void AnimationEditor::Initialize(IResourceManager* const& resourceManager) 
{
    m_myView = dynamic_cast<AnimationEditorView*>(View::SafeGetView(resourceManager, "AnimationView").Get());
    m_curveScene = dynamic_cast<CurveEditor*>(SafeGetController(resourceManager, "CurveEditor").Get());

    assert(m_myView.Valid());
    assert(m_curveScene.Valid());

    // manage animation role
    m_myView->onChannelSelected += Delegate(m_curveScene.Get(), &CurveEditor::ChannelSelectedEvent);
    m_myView->onChannelDeselected += Delegate(m_curveScene.Get(), &CurveEditor::ChannelDeselectedEvent);

    OutlineModule* outlineModue = dynamic_cast<OutlineModule*>(SafeGetController(resourceManager, "OutlineModule").Get());
    assert(outlineModue);
    outlineModue->onItemSelected += Delegate(this, &AnimationEditor::AnimationSelectedEvent);

    Ref<Editor> editor = dynamic_cast<Editor*>(SafeGetController(resourceManager, "Editor").Get());
    assert(editor);

    // manage playback role 
    Timer* timer = editor->GetMusicProxy();
    assert(timer);
    m_myView->onTogglePlayback += Delegate(timer, &Timer::TogglePlay);
    m_myView->onStopPlayback += Delegate(timer, &Timer::Stop);
}

// ===========================================================================================

void AnimationEditor::AnimationSelectedEvent(TreeItem* const item)
{
    ItemHasAnimationsRole* animationItem = dynamic_cast<ItemHasAnimationsRole*>(item);
    if (!animationItem || !animationItem->GetAnimation()) // hide elements
    {
        m_myView->onChannelDeselected();
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
