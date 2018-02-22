#include "curvedoc.h"

#include "curvepointitem.h"

using namespace Idogep;

Idogep::CurveDocument::CurveDocument(Ref<Grafkit::Animation::FloatTrack>& track) : m_track(track), m_curve(nullptr)
{
	trackChanged();
}

void Idogep::CurveDocument::trackChanged()
{
	delete 	m_curve;
	m_curve = new QList<Idogep::CurvePointItem*>();

	size_t keyCount = m_track->GetKeyCount();
	for (size_t i = 0; i < keyCount; i++) {
		auto key = m_track->GetKey(i);
	}
}
