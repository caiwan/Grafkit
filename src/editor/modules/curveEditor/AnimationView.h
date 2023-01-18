#pragma once

#include "animation/animation.h"

#include "utils/ViewModule.h"

namespace Idogep
{
    class CurveCursor;

    class AnimationWrapper;

    typedef Ref<AnimationWrapper> AnimationWrapperRef;

    class TreeModel;

    // ========================================================================================================

    class AnimationEditorView : public View
    {
    public:
        AnimationEditorView() = default;
    
        virtual void UpdateAnimationModel(TreeModel* model) = 0;
        Event<Grafkit::Animation::ChannelRef> onChannelSelected;

        Event<> onTogglePlayback;
        Event<> onStopPlayback;
    };
}
