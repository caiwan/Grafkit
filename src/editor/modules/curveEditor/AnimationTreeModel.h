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

        AnimationChannelItem(const QList<QVariant>& data, TreeItem* parentItem);

        Grafkit::Animation::ChannelRef GetChannel() const { return m_channel; }
        void SetChannel(Grafkit::Animation::ChannelRef& channel) { m_channel = channel; }

    private:
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
