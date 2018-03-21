#pragma once 

#include <qobject.h>

#include "Event.h"

#include "animation/animation.h"

namespace Idogep {

	class CurvePointItem;
	class CurveEditorScene;

	class CurveCursor;

	// Delegate audiogram rendering functions 
	class CurveAudiogram {
	public:
		CurveAudiogram();
		virtual ~CurveAudiogram();
		
		void getAudiogram(QImage** image, float startTime, float endTime, int rectWidth, int rectHeight);
		void clearAudiogram();

	protected:
		virtual bool requestWaveform(float*& p, size_t& sampleCount, size_t& channelCount, size_t& samplePerSec) = 0;

	private:
		float* m_audiogramBuffer;
		size_t m_audiogramSampleCount;

		size_t m_audiogramChannelCount;
		size_t m_audiogramSamplePerSec;
	};

	// --- 

	class CurveDocument : /*: public QObject,*/ public CurveAudiogram
	{
		friend class CurvePointItem;
		friend class CurveEditorScene;

		//Q_OBJECT
	public:
		//explicit CurveDocument(Ref<Grafkit::Animation::FloatTrack>& track, QObject *parent = nullptr);
		CurveDocument();

		QList<CurvePointItem*>* getCurvePoints() { return m_curve; }
		Ref<Grafkit::Animation::Track> getTrack() { return m_track; }

		void setTrack(Ref<Grafkit::Animation::Track>& track);

	protected:
		void recalculate();
		void addCurveToScene(CurveEditorScene* parent);

		virtual void refreshView(bool force = false) = 0;

	private:
		void movePoint(CurvePointItem *item);
		void moveTangent(CurvePointItem *item);
		void startEdit(CurvePointItem *item);
		void commitEdit(CurvePointItem *item);

	private:
		QList<CurvePointItem*>* m_curve;
		Ref<Grafkit::Animation::Track> m_track; // todo: multiple fiszfasz

		// TODO:
		//CurveCursor *m_crsrPlayback;
		//CurveCursor *m_crsrHover;
		//CurveCursor *m_crsrMarkerStart;
		//CurveCursor *m_crsrMarkerEnd;
	};

}