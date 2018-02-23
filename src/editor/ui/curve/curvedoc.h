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

		void trackChanged();
		void recalculate();

		void addCurveToScene(CurveEditorScene* parent);

	private:
		QList<CurvePointItem*>* m_curve;
		Ref<Grafkit::Animation::FloatTrack> &m_track;
	};

}