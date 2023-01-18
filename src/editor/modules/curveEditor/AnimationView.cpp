#include "AnimationView.h"

#include "utils/tree/treeitem.h"
#include "utils/tree/treemodel.h"

using namespace Idogep;
using namespace Grafkit;

// ========================================================================================================

namespace
{
    class AnimationTreeModel : public TreeModel
    {
    public:
        explicit AnimationTreeModel(AnimationRef animation);

        QString GetParentName() const { return m_parentName; }
        void SetParentName(const QString& parentName) { m_parentName = parentName; }

    protected:
        QStringList Header() override;
        void Build(TreeItem* parentItem) override;

        void BuildTrack(TreeItem* const & parentItem, Ref<Animation::Track>& track) const;

    private:
        AnimationRef m_animation;
        QString m_parentName;
    };

    AnimationTreeModel::AnimationTreeModel(AnimationRef animation)
        : m_animation(animation)
    {
    }

    QStringList AnimationTreeModel::Header() { return QStringList() << m_parentName; }

    void AnimationTreeModel::Build(TreeItem* parentItem)
    {
        if (m_animation.Invalid())
            return;

        for (size_t i = 0; i < m_animation->GetTrackCount(); i++)
        {
            Ref<Animation::Track> track = m_animation->GetTrack(i);

            QVariantList data;
            data << QString::fromStdString(track->GetName());
            TreeItem* item = new TreeItem(data, parentItem);

            parentItem->addChild(item);

            BuildTrack(item, track);
        }
    }

    void AnimationTreeModel::BuildTrack(TreeItem* const & parentItem, Ref<Animation::Track>& track) const
    {
        for (size_t i = 0; i < track->GetChannelCount(); i++)
        {
            Ref<Animation::Channel> channel = track->GetChannel(i);

            QVariantList data;
            data << QString::fromStdString(channel->GetName());
            AnimationChannelItem* item = new AnimationChannelItem(data, parentItem);
            item->SetChannel(channel);

            parentItem->addChild(item);
        }
    }
}

// ========================================================================================================

void ManageAnimationsRole::AnimationChangedEvent(AnimationRef animation, const std::string& parentName)
{
	AnimationTreeModel* newModel = new AnimationTreeModel(m_animation);
	newModel->SetParentName(QString::fromStdString(parentName));
	newModel->BuildModel();
    UpdateAnimationModel(newModel);

	delete m_animationListModel;
	m_animationListModel = newModel;
	m_animation = animation;
}
