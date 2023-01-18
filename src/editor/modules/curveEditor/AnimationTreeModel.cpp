#include "AnimationTreeModel.h"
#include "models/Curve.h"

using namespace Idogep;
using namespace Grafkit;

// ========================================================================================================

AnimationChannelItem::AnimationChannelItem(const QList<QVariant>& data, TreeItem* parentItem, const Grafkit::Animation::TrackRef& track, size_t trackId, const Grafkit::Animation::ChannelRef& channel): TreeItem(data, parentItem)
    , m_track(track)
    , m_trackId(trackId)
    , m_channel(channel) {
}
// ========================================================================================================

AnimationTreeModel::AnimationTreeModel(AnimationWrapperRef animation)
    : m_animation(animation)
{
}


QStringList AnimationTreeModel::Header() { return QStringList() << GetParentName(); }

void AnimationTreeModel::Build(TreeItem* parentItem)
{
    if (m_animation.Invalid())
        return;

    AnimationRef animation = m_animation->GetAnimation();
    for (size_t i = 0; i < animation->GetTrackCount(); i++)
    {
        Ref<Animation::Track> track = animation->GetTrack(i);

        QVariantList data;
        data << QString::fromStdString(track->GetName());
        TreeItem* item = new TreeItem(data, parentItem);

        parentItem->addChild(item);

        BuildTrack(item, track);
    }
}

void AnimationTreeModel::BuildTrack(TreeItem * const & parentItem, Animation::TrackRef & track)
{
    for (size_t i = 0; i < track->GetChannelCount(); i++)
    {
        Animation::ChannelRef channel = track->GetChannel(i);

        QVariantList data;
        data << QString::fromStdString(channel->GetName());
        AnimationChannelItem* item = new AnimationChannelItem(data, parentItem, track, i, track->GetChannel(i));
        parentItem->addChild(item);
    }
}


