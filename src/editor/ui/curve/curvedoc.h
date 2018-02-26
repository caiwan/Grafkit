#pragma once 

#include <qobject.h>

#include "Event.h"

#include "render/animation.h"

namespace Idogep {

	class CurvePointItem;
	class CurveEditorScene;

	class CurveAudiogram {
	public:
		CurveAudiogram();
		virtual ~CurveAudiogram();

		Event<float*&, size_t&, size_t&, size_t&> onRequestWaveform; // ptr, samplecount, channelcount, samplePerSec
		void getAudiogram(QImage** image, float startTime, float endTime, int rectWidth, int rectHeight);

	private:
		float* m_audiogramBuffer;
		size_t m_audiogramSampleCount;

		size_t m_audiogramChannelCount;
		size_t m_audiogramSamplePerSec;
	};

	class CurveDocument : public QObject, public CurveAudiogram
	{
		Q_OBJECT
	public:
		explicit CurveDocument(Ref<Grafkit::Animation::FloatTrack>& track, QObject *parent = nullptr);

		QList<CurvePointItem*>* getCurvePoints() { return m_curve; }
		Ref<Grafkit::Animation::FloatTrack> getTrack() { return m_track; }

		void setTrack(Ref<Grafkit::Animation::FloatTrack>& track);

		void recalculate();
		void addCurveToScene(CurveEditorScene* parent);

	private:
		void onMovePoint(CurvePointItem *item);
		void onMoveTangent(CurvePointItem *item);
		void onStartEdit(CurvePointItem *item);
		void onCommitEdit(CurvePointItem *item);

	private:
		QList<CurvePointItem*>* m_curve;
		Ref<Grafkit::Animation::FloatTrack> m_track; // todo: multiple fiszfasz

	};

}