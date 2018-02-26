#pragma once 

#include <qobject.h>

#include "render/animation.h"

namespace Idogep {

	class CurvePointItem;
	class CurveEditorScene;

	class CurveDocument : public QObject
	{

	public:

		CurveDocument(Ref<Grafkit::Animation::FloatTrack>& track, QObject *parent = nullptr);

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