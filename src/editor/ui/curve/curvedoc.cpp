#include "curvedoc.h"

#include "curveeditorscene.h"
#include "curvepointitem.h"

using namespace Idogep;

Idogep::CurveAudiogram::CurveAudiogram() : m_audiogramSampleCount(0), m_audiogramBuffer(nullptr)
{
}

Idogep::CurveAudiogram::~CurveAudiogram()
{
	delete m_audiogramBuffer;
}

void Idogep::CurveAudiogram::getAudiogram(QImage ** image, float startTime, float endTime, int rectWidth, int rectHeight)
{
	*image = nullptr;

	// todo: update when audiogramChanged
	if (!m_audiogramBuffer) {
		onRequestWaveform(m_audiogramBuffer, m_audiogramSampleCount, m_audiogramChannelCount, m_audiogramSamplePerSec);
	}

	uint32_t offset = uint32_t(startTime * m_audiogramSamplePerSec) * m_audiogramChannelCount;
	uint32_t length = uint32_t((endTime - startTime) * m_audiogramSamplePerSec) * m_audiogramChannelCount;

	float* buffer = &m_audiogramBuffer[offset];

	QImage* img = new QImage(QSize(rectWidth, rectHeight), QImage::Format_ARGB32);
	img->fill(0xFF303030);

	uint32_t readFrames = m_audiogramSampleCount;
	uint32_t valueHop = (readFrames / rectWidth) + ((readFrames % rectWidth) % 2);

	float invChannelCount;

	uint32_t fi = 0;
	for (uint32_t i = 0; i < readFrames * 2; i += valueHop * 2)
	{
		if (fi >= rectWidth) break;
		float vv = 0.0f;
		for (uint32_t k = 0; k < valueHop * 2; k += 2)
		{
			float sum = 0.0f;
			if ((i + (k / 2) + 1) >= length)
			{
				sum = 0.;
			}
			else {
				float * p = &buffer[i + (k / 2)];
				for (uint32_t l = 0; l < m_audiogramChannelCount; l++) {
					sum += fabs(p[l]);
				}
			}
		}

		vv /= float(valueHop) / invChannelCount;

		for (uint32_t j = 0; j < rectHeight; j++)
		{
			float cv = fabs((float(j)*2.0f) / float(rectHeight - 1) - 1.0f);
			if (vv >= cv) img->setPixel(fi, j, 0xFF3c3c3c);
		}

		fi++;
	}
	*image = img;
}



Idogep::CurveDocument::CurveDocument(Ref<Grafkit::Animation::FloatTrack>& track, QObject *parent) :
	QObject(parent),
	CurveAudiogram(),
	m_curve(nullptr)
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

		// TODO:  ezt a kettot
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

