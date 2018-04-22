#include "curvedoc.h"

#include "utils/Command.h"
#include "commands/curveCommands.h"

#include "curveeditorscene.h"
#include "curvepointitem.h"

using namespace Idogep;

Idogep::ManageCurveAudiogramRole::ManageCurveAudiogramRole() : m_audiogramSampleCount(0), m_audiogramBuffer(nullptr)
{
}

Idogep::ManageCurveAudiogramRole::~ManageCurveAudiogramRole()
{
	delete m_audiogramBuffer;
}


void Idogep::ManageCurveAudiogramRole::GetAudiogram(QImage ** image, float startTime, float endTime, int rectWidth, int rectHeight)
{
	*image = nullptr;

	// todo: update when audiogramChanged
	if (!m_audiogramBuffer) {
		RequestWaveform(m_audiogramBuffer, m_audiogramSampleCount, m_audiogramChannelCount, m_audiogramSamplePerSec);
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

void Idogep::ManageCurveAudiogramRole::ClearAudiogram()
{
	delete m_audiogramBuffer;
	m_audiogramBuffer = nullptr;
}



Idogep::ManageCurveRole::ManageCurveRole() :
	ManageCurveAudiogramRole(),
	m_curve(nullptr)
{
}

void Idogep::ManageCurveRole::SetChannel(Ref<Grafkit::Animation::Channel>& channel)
{

	m_channel = channel;

	//delete m_curve;
	if (m_curve)
		m_curve->clear();

	m_curve = new QList<Idogep::CurvePointItem*>();

	size_t keyCount = m_channel->GetKeyCount();
	for (size_t i = 0; i < keyCount; i++) {
		auto key = m_channel->GetKey(i);
		CurvePointItem *point = new CurvePointItem(key, i);
		m_curve->push_back(point);

		point->onStartEdit += Delegate(this, &ManageCurveRole::StartEdit);
		point->onCommitEdit += Delegate(this, &ManageCurveRole::CommitEdit);
		point->onMovePoint += Delegate(this, &ManageCurveRole::MovePoint);
		point->onMoveTangent += Delegate(this, &ManageCurveRole::MoveTangent);
	}
}

void Idogep::ManageCurveRole::Recalculate()
{
	if (m_curve) {
		for (int i = 0; i < m_curve->size(); i++) {
			m_curve->at(i)->recalculatePosition();
			// if you set points visibilityduring the drawing sequence, it will not recieve any events
		}
	}
}

void Idogep::ManageCurveRole::AddCurveToScene(CurveEditorScene * parent)
{
	if (m_curve) {
		for (int i = 0; i < m_curve->size(); i++) {
			auto point = m_curve->at(i);
			parent->addItem(point);
			point->recalculatePosition();
			// if you set points visibilityduring the drawing sequence, it will not recieve any events
		}
	}
}

void Idogep::ManageCurveRole::MovePoint(CurvePointItem * item)
{
	float itemTime = item->time();
	uint32_t id = item->index();

	if (id > 0) {
		auto prevKey = m_channel->GetKey(id - 1);
		if (itemTime <= prevKey.m_time)
			item->setTime(prevKey.m_time + (1. / 120.));
	}

	if (id < m_channel->GetKeyCount() - 1) {
		auto nextKey = m_channel->GetKey(id + 1);
		if (itemTime > nextKey.m_time)
			item->setTime(nextKey.m_time - (1. / 120.));
	}

	if (itemTime < 0.)
		item->setTime(0);

	m_channel->SetKey(id, item->GetKey());
}

void Idogep::ManageCurveRole::MoveTangent(CurvePointItem * item)
{
	// ... 
}

void Idogep::ManageCurveRole::StartEdit(CurvePointItem * item)
{
	m_originalKey = item->GetKey();
}

void Idogep::ManageCurveRole::CommitEdit(CurvePointItem * item)
{
	m_modifiedKey = item->GetKey();
	CurveKeyChangeCommand * cmd = new CurveKeyChangeCommand(m_channel, item->index(), m_originalKey, m_modifiedKey, nullptr);
	//cmd->onRefreshView += Delegate(this, &ManageCurveRole::RefreshView);
	onNewCommand(cmd);
}

void Idogep::ManageCurveRole::CommitAddPoint(float key, float value)
{
}

void Idogep::ManageCurveRole::CommitRemovePoint(float key, float value)
{
}

