#pragma once 

#include <QtGui>
#include <qdockwidget.h>

#include "animation/animation.h"

#include "utils/tree/treeitem.h"
#include "utils/Event.h"
#include "models/ViewModule.h"

#include "curvedoc.h"

namespace Ui {
	class CurveEditorWidget;
}

class QTreeView;

namespace Idogep {

	class TreeModel;
	class TreeItem;

	class CurveEditorScene;
	class CurvePointItem;
	class ManageCurveRole;

	class CurveCursor;

	// ========================================================================================================

	class AnimationChannelItem : public TreeItem {
	public:
		AnimationChannelItem(const QList<QVariant> &data, TreeItem *parentItem = nullptr);

		Ref<Grafkit::Animation::Channel> GetChannel() { return m_channel; }
		void SetChannel(Ref<Grafkit::Animation::Channel> &channel) { m_channel = channel; }

	private:
		Ref<Grafkit::Animation::Channel> m_channel;
	};

	// --------------------------------------------------------------------------------------------------------- 

	class ManageAnimationsRole {
	public:
		ManageAnimationsRole();

		void AnimationChangedEvent(Grafkit::AnimationRef animation);

	protected:
		Event<Ref<Grafkit::Animation::Channel>> onChannelSelected;
		virtual void UpdateAnimationModel(TreeModel* model) = 0;

	protected:
		Grafkit::AnimationRef m_animation;

		TreeModel * m_animationListModel;
	};

	// ========================================================================================================

	class ManagePlaybackRole {
	public:
		// Play pressed
		// Stop pressed

		// Update cursor from player

		// --- intenral events (triggered from inside)
		Event<> onPlaybackOptions;

		Event<> onTogglePlayback;
		Event<> onStopPlayback;

		Event<float*&, size_t&, size_t&, size_t&> onRequestWaveform; // ptr, samplecount, channelcount, samplePerSec

		// --- external events (triggered from ouitside)
		virtual void musicChanged() = 0;

		virtual void playbackChanged(bool isPlaying) = 0;
		virtual void demoTimeChanged(float time) = 0;

	protected:
		// ... 

	};

	// ========================================================================================================

	class CurveEditorWidget :
		public QDockWidget,
		public View,
		public ManageCurveRole, public ManageAnimationsRole, public ManagePlaybackRole
	{
		Q_OBJECT

	public:
		explicit CurveEditorWidget(QWidget *parent = 0);
		~CurveEditorWidget();

		/* EVENTS and OPERATIONS */
	public:

		virtual void playbackChanged(bool isPlaying);
		virtual void demoTimeChanged(float time);

		virtual void musicChanged() {}

		/* METHODS */
	protected:
		virtual void resizeEvent(QResizeEvent* event);
		virtual void RefreshView(bool force);

		virtual bool RequestWaveform(float*& p, size_t& sampleCount, size_t& channelCount, size_t& samplePerSec) { onRequestWaveform(p, sampleCount, channelCount, samplePerSec); return sampleCount > 0; }

		//void onAnimationModelUpdatedEvent(TreeModel* model);
		virtual void UpdateAnimationModel(TreeModel* model);

		protected slots:
		void playPressedSlot() { onTogglePlayback(); }
		void stopPressedSlot() { onStopPlayback(); }
		void optionsPressedSlot() { onPlaybackOptions(); }

		// ... 
		void itemClickedSlot(const QModelIndex &index);
		void itemDoubleClickedSlot(const QModelIndex &index);


	private:
		Ui::CurveEditorWidget *ui;
		CurveEditorScene* m_scene;

	};


}
