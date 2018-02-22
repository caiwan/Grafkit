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
		CurvePointItem *point = new CurvePointItem();
		point->setTime(key.m_key);
		point->setValue(key.m_value);
		m_curve->push_back(point);
	}
}

void Idogep::CurveDocument::recalculate()
{
	if (m_curve) {
		for (int i = 0; i < m_curve->size(); i++) {
			m_curve->at(i)->recalculatePosition();
		}
	}

	//while (!unsortedPointItems.isEmpty()) {
	//	CurvePointItem* minPoint = NULL;
	//	float minTime = 9999.0f;
	//	for (int i = 0; i < unsortedPointItems.size(); i++) {
	//		if (unsortedPointItems[i]->time() < minTime) {
	//			minPoint = unsortedPointItems[i];
	//			minTime = unsortedPointItems[i]->time();
	//		}
	//	}
	//	sortedPointItems.append(minPoint);
	//	unsortedPointItems.removeOne(minPoint);
	//}

}
