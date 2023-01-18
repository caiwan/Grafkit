#pragma once

#include "utils/ViewModule.h"

#include "animation/animation.h"

#include "utils/tree/treeitem.h"
#include "utils/tree/treemodel.h"

namespace  Idogep
{
	class CurveCursor;

	class TreeModel;
	class TreeItem;

	// ========================================================================================================

	class AnimationChannelItem : public TreeItem {
	public:
		explicit AnimationChannelItem(const QList<QVariant> &data, TreeItem *parentItem = nullptr);

		Ref<Grafkit::Animation::Channel> GetChannel() const { return m_channel; }
		void SetChannel(Ref<Grafkit::Animation::Channel> &channel) { m_channel = channel; }

	private:
		Ref<Grafkit::Animation::Channel> m_channel;
	};

	// --------------------------------------------------------------------------------------------------------- 

	class ManageAnimationsRole {
	public:
		ManageAnimationsRole();
		virtual ~ManageAnimationsRole() = default;

		void AnimationChangedEvent(Grafkit::AnimationRef animation);

	protected:
		Event<Ref<Grafkit::Animation::Channel>> onChannelSelected;
		virtual void UpdateAnimationModel(TreeModel* model) = 0;

		Grafkit::AnimationRef m_animation;
		TreeModel * m_animationListModel;
	};

	// ========================================================================================================

	class ManagePlaybackRole {
	public:
		ManagePlaybackRole() {}
		virtual ~ManagePlaybackRole() = default;

		// Play pressed
		// Stop pressed

		// Update cursor from player

		// --- intenral events (triggered from inside)
		Event<> onPlaybackOptions;

		Event<> onTogglePlayback;
		Event<> onStopPlayback;

		Event<float*&, size_t&, size_t&, size_t&> onRequestWaveform; // ptr, samplecount, channelcount, samplePerSec

																	 // --- external events (triggered from ouitside)
		virtual void MusicChanged() = 0;

		virtual void PlaybackChanged(bool isPlaying) = 0;
		virtual void demoTimeChanged(float time) = 0;

	};

	// ========================================================================================================


	class CurveEditorView : public View, public ManagePlaybackRole, public ManageAnimationsRole
	{
		// ... 
	};
}
