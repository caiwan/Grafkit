#pragma once

#include "animation/animation.h"

#include "utils/ViewModule.h"
#include "utils/Command.h"

namespace  Idogep
{
	class AnimationEditorView;
	class CurveEditor;
    class CurveCursor;

	class TreeItem;
    class TreeModel;

    class AnimationWrapper;
    typedef Ref<AnimationWrapper> AnimationWrapperRef;

	// ========================================================================================================

	class AnimationEditor : public Controller, public EmitsCommandRole
	{
	public:
		explicit AnimationEditor();
		~AnimationEditor() override;

	    void Initialize(Grafkit::IResourceManager* const& resourceManager) override;

		Event<float*&, size_t&, size_t&, size_t&> onRequestWaveform; // ptr, samplecount, channelcount, samplePerSec

		void AnimationSelectedEvent(TreeItem * const item);

	private:
		Ref<AnimationEditorView> m_myView;
		Ref<CurveEditor> m_curveScene;

        AnimationWrapperRef m_animation;
        
        TreeModel* m_animationListModel;
	};

    // ========================================================================================================

    class AnimationEditorView : public View
    {
    public:
        AnimationEditorView();

        virtual void UpdateAnimationModel(TreeModel* model) = 0;
        Event<Grafkit::Animation::TrackRef&, const size_t&, Grafkit::Animation::ChannelRef&> onChannelSelected;
        Event<> onChannelDeselected;

        Event<> onTogglePlayback;
        Event<> onStopPlayback;
    };

}
