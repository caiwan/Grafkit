#pragma once 

#include "render/animation.h"

#include "curvepointitem.h"

namespace Idogep {

	class CurveDocument {
	public:

		CurveDocument(Ref<Grafkit::Animation::FloatTrack>& track);

		QList<CurvePointItem*>* getCurve() { return m_curve; }
		Ref<Grafkit::Animation::FloatTrack> getTrack() { return m_track; }

		void trackChanged();

	private:
		QList<CurvePointItem*>* m_curve;
		Ref<Grafkit::Animation::FloatTrack> &m_track;
	};

}