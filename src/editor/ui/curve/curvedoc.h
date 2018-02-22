#pragma once 

#include "render/animation.h"

//#include "curvepointitem.h"

// ossze kell vonni a curveeditorscenenel

namespace Idogep {

	class CurvePointItem;
	class CurveEditorScene;

	class CurveDocument {
	public:

		// add parent shit here 
		CurveDocument(Ref<Grafkit::Animation::FloatTrack>& track);

		QList<CurvePointItem*>* getCurvePoints() { return m_curve; }
		Ref<Grafkit::Animation::FloatTrack> getTrack() { return m_track; }

		void trackChanged();
		void recalculate();

	private:
		QList<CurvePointItem*>* m_curve;
		Ref<Grafkit::Animation::FloatTrack> &m_track;
	};

}