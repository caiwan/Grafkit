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

    class PointEditorView : virtual public View
    {
    public:
        PointEditorView();

        // events and stuff 

        // set point
        void PointSet(size_t id, const Grafkit::Animation::Key&);
        void PointUnSet();

        // changed event
        size_t GetId() const { return m_id; }
        void SetId(const size_t id) { m_id = id; }

        Grafkit::Animation::Key GetKey() const { return m_key; }
        void SetKey(const Grafkit::Animation::Key& key) { m_key = key; }

    protected:
        //virtual void UpdatePointEditorView() = 0;

    private:
        size_t m_id;
        Grafkit::Animation::Key m_key;
    };
}
