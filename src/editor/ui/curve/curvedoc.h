#pragma once 

#include <qobject.h>

#include "animation/animation.h"
#include "Event.h"
#include "Command.h"

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
		// TODO:
		//CurveCursor *m_crsrPlayback;
		//CurveCursor *m_crsrHover;
		//CurveCursor *m_crsrMarkerStart;
		//CurveCursor *m_crsrMarkerEnd;
	};

	// --- 

	class ManageCurveRole : public ManageCurveAudiogramRole, public ManageCursorRole, public EmitsCommandRole
	{
		friend class CurvePointItem;
		friend class CurveEditorScene;

		//Q_OBJECT
	public:
		ManageCurveRole();

		QList<CurvePointItem*>* GetCurvePoints() { return m_curve; }
		Ref<Grafkit::Animation::Channel> GetChannel() { return m_track; }

		void SetChannel(Ref<Grafkit::Animation::Channel>& track);

	protected:
		void Recalculate();
		void AddCurveToScene(CurveEditorScene* parent);

		virtual void RefreshView(bool force = false) = 0;

	protected:
		void MovePoint(CurvePointItem *item);
		void MoveTangent(CurvePointItem *item);
		//void StartEdit(CurvePointItem *item);
		void CommitEdit(CurvePointItem *item);

		void CommitAddPoint(float key, float value);
		void CommitRemovePoint(float key, float value);

	private:
		QList<CurvePointItem*>* m_curve;
		Ref<Grafkit::Animation::Channel> m_track; // todo: multiple fiszfasz


	};

}