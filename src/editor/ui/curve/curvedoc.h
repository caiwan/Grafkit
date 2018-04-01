#pragma once 

#include <qobject.h>

#include "Event.h"

#include "animation/animation.h"

namespace Idogep {

	class CurvePointItem;
	class CurveEditorScene;

	class CurveCursor;

	// Delegate audiogram rendering functions 
	class ManageCurveAudiogramRole {
	public:
		ManageCurveAudiogramRole();
		virtual ~ManageCurveAudiogramRole();
		
		void GetAudiogram(QImage** image, float startTime, float endTime, int rectWidth, int rectHeight);
		void ClearAudiogram();

	protected:
		virtual bool RequestWaveform(float*& p, size_t& sampleCount, size_t& channelCount, size_t& samplePerSec) = 0;

	private:
		float* m_audiogramBuffer;
		size_t m_audiogramSampleCount;

		size_t m_audiogramChannelCount;
		size_t m_audiogramSamplePerSec;
	};

	// --- 

	class ManageCursorRole {

	};

	// --- 

	class ManageCurveRole : /*: public QObject,*/ public ManageCurveAudiogramRole, public ManageCursorRole
	{
		friend class CurvePointItem;
		friend class CurveEditorScene;

		//Q_OBJECT
	public:
		//explicit CurveDocument(Ref<Grafkit::Animation::FloatTrack>& track, QObject *parent = nullptr);
		ManageCurveRole();

		QList<CurvePointItem*>* GetCurvePoints() { return m_curve; }
		Ref<Grafkit::Animation::Channel> GetChannel() { return m_track; }

		void SetChannel(Ref<Grafkit::Animation::Channel>& track);

	protected:
		void Recalculate();
		void AddCurveToScene(CurveEditorScene* parent);

		virtual void RefreshView(bool force = false) = 0;

	private:
		void MovePoint(CurvePointItem *item);
		void MoveTangent(CurvePointItem *item);
		void StartEdit(CurvePointItem *item);
		void CommitEdit(CurvePointItem *item);

	private:
		QList<CurvePointItem*>* m_curve;
		Ref<Grafkit::Animation::Channel> m_track; // todo: multiple fiszfasz

		// TODO:
		//CurveCursor *m_crsrPlayback;
		//CurveCursor *m_crsrHover;
		//CurveCursor *m_crsrMarkerStart;
		//CurveCursor *m_crsrMarkerEnd;
	};

}