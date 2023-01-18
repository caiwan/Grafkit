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

    // ========================================================================================================

    class PointEditorView : virtual public View
    {
    public:
        PointEditorView();

        virtual void UpdatePointEditor(bool isSet) = 0;

        size_t GetPointId() const { return m_pointId; }
        void SetPointId(const size_t pointId) { m_pointId = pointId; }

        Grafkit::Animation::Key GetPointKey() const { return m_pointKey; }
        void SetPointKey(const Grafkit::Animation::Key& pointKey) { m_pointKey = pointKey; }

        Event<size_t, Grafkit::Animation::Key> onEditKeyEvent;

    protected:
        size_t m_pointId;
        Grafkit::Animation::Key m_pointKey;
    };

}
