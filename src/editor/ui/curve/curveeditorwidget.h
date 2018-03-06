#pragma once 

#include <QtGui>
#include <qdockwidget.h>

#include "Event.h"

#include "curvedoc.h"

namespace Ui {
	class CurveEditorWidget;
}

namespace Idogep {

	class CurveEditorScene;
	class CurvePointItem;
	class CurveDocument;

	// --- 
	class CurveEditorWidget : public QDockWidget, public CurveDocument
	{
		friend 	class CurveEditorScene;
		friend class CurvePointItem;
		friend class CurveDocument;
		Q_OBJECT

	public:
		explicit CurveEditorWidget(QWidget *parent = 0);
		~CurveEditorWidget();

		/* EVENTS and OPERATIONS */
	public:
		// --- intenral events (triggered from inside)
		Event<> onPlaybackOptions;

		Event<> onTogglePlayback;
		Event<> onStopPlayback;

		Event<float*&, size_t&, size_t&, size_t&> onRequestWaveform; // ptr, samplecount, channelcount, samplePerSec

		// --- external events (triggered from ouitside)
		void musicChanged() { refreshView(true); }

		void playbackChanged(bool isPlaying);
		void demoTimeChanged(float time);

		/* METHODS */
	protected:
		virtual void resizeEvent(QResizeEvent* event);
		virtual void refreshView(bool force);

		virtual bool requestWaveform(float*& p, size_t& sampleCount, size_t& channelCount, size_t& samplePerSec) { onRequestWaveform(p, sampleCount, channelCount, samplePerSec); return sampleCount > 0; }

		protected slots:
		void playPressedSlot() { onTogglePlayback(); }
		void stopPressedSlot() { onStopPlayback(); }
		void optionsPressedSlot() { onPlaybackOptions(); }

	private:
		Ui::CurveEditorWidget *ui;
		CurveEditorScene* m_scene;

	};


}
