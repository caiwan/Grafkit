#pragma once 

#include <QtGui>
#include <qdockwidget.h>

#include "animation/animation.h"

#include "CurveView.h"

#include "curvedoc.h"

#include "CurveModule.h"

namespace Ui {
	class CurveEditorWidget;
}

class QTreeView;
class QGraphicsScene;

namespace Idogep {

	class CurveCursor;

	// ========================================================================================================

	class CurveEditorWidget :
		public QDockWidget,
		public CurveEditorView
	{
		Q_OBJECT

	public:
		explicit CurveEditorWidget(QWidget *parent = 0);
		~CurveEditorWidget();

		/* METHODS */
		void SetGraphicsScene(QGraphicsScene* graphicsScene);

	protected:
	 
	    void RefreshView(bool force) override;

		//virtual bool RequestWaveform(float*& p, size_t& sampleCount, size_t& channelCount, size_t& samplePerSec) { onRequestWaveform(p, sampleCount, channelCount, samplePerSec); return sampleCount > 0; }

		//void onAnimationModelUpdatedEvent(TreeModel* model);
		virtual void UpdateAnimationModel(TreeModel* model);

        // QT specific
		protected slots:
	    // ReSharper disable CppInconsistentNaming
		void playPressedSlot() { onTogglePlayback(); }
		void stopPressedSlot() { onStopPlayback(); }
		//void optionsPressedSlot() { onPlaybackOptions(); }

		// ... 
		void itemClickedSlot(const QModelIndex &index);
		void itemDoubleClickedSlot(const QModelIndex &index);


	protected:
	    void resizeEvent(QResizeEvent* event) override;
	    // ReSharper restore CppInconsistentNaming

	private:
		Ui::CurveEditorWidget *ui;
		//CurveEditorScene* m_scene;

	};

}
