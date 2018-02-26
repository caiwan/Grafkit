#include "curvedoc.h"

#include "curveeditorscene.h"
#include "curvepointitem.h"

using namespace Idogep;

Idogep::CurveDocument::CurveDocument(Ref<Grafkit::Animation::FloatTrack>& track, QObject *parent) :
	QObject(parent), m_curve(nullptr)
{
	setTrack(track);
}

void Idogep::CurveDocument::setTrack(Ref<Grafkit::Animation::FloatTrack>& track)
{
	m_track = new Grafkit::Animation::FloatTrack(track);

	// clear?
	delete 	m_curve;
	m_curve = new QList<Idogep::CurvePointItem*>();

	size_t keyCount = m_track->GetKeyCount();
	for (size_t i = 0; i < keyCount; i++) {
		auto key = m_track->GetKey(i);
		CurvePointItem *point = new CurvePointItem();
		point->setIndex(i);

		point->setTime(key.m_key);
		point->setValue(key.m_value);

		//point->setColor();
		//point->setNodeType();

		point->onMovePoint += Delegate(this, &CurveDocument::onMovePoint);

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
}

void Idogep::CurveDocument::addCurveToScene(CurveEditorScene * parent)
{
	if (m_curve) {
		for (int i = 0; i < m_curve->size(); i++) {
			auto point = m_curve->at(i);
			parent->addItem(point);
			point->recalculatePosition();
		}
	}
}

void Idogep::CurveDocument::onMovePoint(CurvePointItem * item)
{
	float itemTime = item->time();
	uint32_t id = item->index();
	uint32_t sid = item->subIndex();

	if (id > 0) {
		auto prevKey = m_track->GetKey(id - 1);
		if (itemTime <= prevKey.m_key)
			item->setTime(prevKey.m_key + (1. / 120.));
	}

	if (id < m_track->GetKeyCount() - 1) {
		auto nextKey = m_track->GetKey(id + 1);
		if (itemTime > nextKey.m_key)
			item->setTime(nextKey.m_key - (1. / 120.));
	}

	if (itemTime < 0.)
		item->setTime(0);

	auto key = m_track->GetKey(id);
	key.m_key = item->time();
	key.m_value = item->value();
	m_track->SetKey(key, id);
}

void Idogep::CurveDocument::onMoveTangent(CurvePointItem * item)
{
	// ... 
}

void Idogep::CurveDocument::onStartEdit(CurvePointItem * item)
{
	// save current state
}

void Idogep::CurveDocument::onCommitEdit(CurvePointItem * item)
{
	// add command to commit it 
}

