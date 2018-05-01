#include "CurveView.h"

#include "utils/tree/treeitem.h"
#include "utils/tree/treemodel.h"

#include "curveeditorwidget.h"

using namespace Idogep;
using namespace Grafkit;

// ========================================================================================================

namespace {
	class AnimationTreeModel : public TreeModel {
	public:
		AnimationTreeModel(Grafkit::AnimationRef animation);

	protected:
		virtual QStringList Header();
		virtual	void Build(TreeItem * parentItem);

	protected:
		void BuildTrack(TreeItem * const &parentItem, Ref<Animation::Track>& track);

	private:
		Grafkit::AnimationRef m_animation;
	};
	AnimationTreeModel::AnimationTreeModel(Grafkit::AnimationRef animation) : m_animation(animation)
	{
	}

	QStringList AnimationTreeModel::Header()
	{
		return QStringList() << "Channel";
	}

	void AnimationTreeModel::Build(TreeItem * parentItem)
	{
		if (m_animation.Invalid())
			return;

		for (size_t i = 0; i < m_animation->GetTrackCount(); i++) {
			Ref<Animation::Track> track = m_animation->GetTrack(i);

			QVariantList data;
			data << QString::fromStdString(track->GetName());
			TreeItem *item = new TreeItem(data, parentItem);

			parentItem->addChild(item);

			BuildTrack(item, track);
		}
	}
	void AnimationTreeModel::BuildTrack(TreeItem * const & parentItem, Ref<Animation::Track> &track)
	{
		for (size_t i = 0; i < track->GetChannelCount(); i++) {
			Ref<Animation::Channel> channel = track->GetChannel(i);

			QVariantList data;
			data << QString::fromStdString(channel->GetName());
			AnimationChannelItem *item = new AnimationChannelItem(data, parentItem);
			item->SetChannel(channel);

			parentItem->addChild(item);
		}
	}
}

// ========================================================================================================

void Idogep::ManageAnimationsRole::AnimationChangedEvent(Grafkit::AnimationRef animation)
{
	// ez majd a model dolga lesz

	m_animation = animation;
	TreeModel * oldModel = m_animationListModel;
	m_animationListModel = new AnimationTreeModel(m_animation);
	m_animationListModel->BuildModel();
	UpdateAnimationModel(m_animationListModel);
	if (oldModel)
		delete oldModel;
}
