#pragma once

#include "models/curve.h"

#include "utils/tree/treeitem.h"
#include "utils/tree/treemodel.h"

#include "animation/animation.h"

namespace Idogep
{
    class TreeModel;
    class TreeItem;

    class AnimationWrapper;
    typedef Ref<AnimationWrapper> AnimationWrapperRef;

    class AnimationChannelItem : public TreeItem {
    public:

        AnimationChannelItem(const QList<QVariant>& data, TreeItem* parentItem, const Grafkit::Animation::TrackRef& track, size_t trackId, const Grafkit::Animation::ChannelRef& channel);

        Grafkit::Animation::TrackRef GetTrack() const { return m_track; }
        size_t GetTrackId() const { return m_trackId; }
        Grafkit::Animation::ChannelRef GetChannel() const { return m_channel; }

    private:
        Grafkit::Animation::TrackRef m_track;
        size_t m_trackId;
        Grafkit::Animation::ChannelRef m_channel;
    };

    // ========================================================================================================


    class AnimationTreeModel : public TreeModel
    {
    public:
        explicit AnimationTreeModel(AnimationWrapperRef animation);

        QString GetParentName() const { return QString::fromStdString(m_animation->GetParentName()); }

    protected:

        QStringList Header() override;
        void Build(TreeItem* parentItem) override;

        static void BuildTrack(TreeItem* const & parentItem, Grafkit::Animation::TrackRef& track);

    private:
        AnimationWrapperRef m_animation;
    };

}
