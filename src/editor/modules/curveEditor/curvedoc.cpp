#include "curvedoc.h"

#include "utils/Command.h"
#include "commands/curveCommands.h"

#include "curveeditorscene.h"
#include "curvepointitem.h"

using namespace Idogep;

void Roles::EditCurveRole::StartEdit(CurvePointItem* item)
{
}

void Roles::EditCurveRole::CommitEdit(CurvePointItem* item)
{
}

void Roles::EditCurveRole::CommitAddPoint(float key, float value)
{
}

void Roles::EditCurveRole::CommitRemovePoint(float key, float value)
{
}

// ========================================================================

void CurveManager::Rebuild()
{
	//delete m_curvePoints;
	if (m_curvePoints)
		m_curvePoints->clear();
	else
		m_curvePoints = new QList<Idogep::CurvePointItem*>();


	size_t keyCount = m_channel->GetKeyCount();
	for (size_t i = 0; i < keyCount; i++) {
		const auto key = m_channel->GetKey(i);
		CurvePointItem *point = new CurvePointItem(key, i);
		m_curvePoints->push_back(point);

#if 0
		point->onStartEdit += Delegate(this, &ManageCurveRole::StartEdit);
		point->onCommitEdit += Delegate(this, &ManageCurveRole::CommitEdit);
		point->onMovePoint += Delegate(this, &ManageCurveRole::MovePoint);
		point->onMoveTangent += Delegate(this, &ManageCurveRole::MoveTangent);
#endif 
	}
}

void CurveManager::Recalculate(TimelineArea* const area)
{


}

void CurveManager::AddCurveToScene(CurveSceneView* parent)
{
	if (!m_curvePoints)
		return;

	for (size_t i = 0; i < m_curvePoints->size(); i++)
	{
		parent->AddCurvePoint(m_curvePoints->at(i));
	}
}


#if 0
Idogep::ManageCurveRole::ManageCurveRole() :
	ManageCurveAudiogramRole(),
	m_curvePoints(nullptr)
{
}

void Idogep::ManageCurveRole::SetChannel(Ref<Grafkit::Animation::Channel>& channel)
{

	m_channel = channel;

	//delete m_curvePoints;
	if (m_curvePoints)
		m_curvePoints->clear();

	m_curvePoints = new QList<Idogep::CurvePointItem*>();

#if 0

	size_t keyCount = m_channel->GetKeyCount();
	for (size_t i = 0; i < keyCount; i++) {
		auto key = m_channel->GetKey(i);
		CurvePointItem *point = new CurvePointItem(key, i);
		m_curvePoints->push_back(point);

		point->onStartEdit += Delegate(this, &ManageCurveRole::StartEdit);
		point->onCommitEdit += Delegate(this, &ManageCurveRole::CommitEdit);
		point->onMovePoint += Delegate(this, &ManageCurveRole::MovePoint);
		point->onMoveTangent += Delegate(this, &ManageCurveRole::MoveTangent);
	}

#endif 
}

void Idogep::ManageCurveRole::Recalculate()
{
#if 0
	if (m_curvePoints) {
		for (int i = 0; i < m_curvePoints->size(); i++) {
			m_curvePoints->at(i)->recalculatePosition();
			// if you set points visibilityduring the drawing sequence, it will not recieve any events
		}
	}
#endif 
}

void Idogep::ManageCurveRole::AddCurveToScene(CurveEditorScene * parent)
{
#if 0
	if (m_curvePoints) {
		for (int i = 0; i < m_curvePoints->size(); i++) {
			auto point = m_curvePoints->at(i);
			parent->addItem(point);
			point->recalculatePosition();
			// if you set points visibilityduring the drawing sequence, it will not recieve any events
		}
	}
#endif 
}

void Idogep::ManageCurveRole::MovePoint(CurvePointItem * item)
{
#if 0
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
#endif 
}

void Idogep::ManageCurveRole::MoveTangent(CurvePointItem * item)
{
	// ... 
}

void Idogep::ManageCurveRole::StartEdit(CurvePointItem * item)
{
#if 0
	m_originalKey = item->GetKey();
#endif 0
}

void Idogep::ManageCurveRole::CommitEdit(CurvePointItem * item)
{
#if 0
	m_modifiedKey = item->GetKey();
	CurveKeyChangeCommand * cmd = new CurveKeyChangeCommand(m_channel, item->index(), m_originalKey, m_modifiedKey, nullptr);
	//cmd->onRefreshView += Delegate(this, &ManageCurveRole::RefreshView);
	onNewCommand(cmd);
#endif
}

void Idogep::ManageCurveRole::CommitAddPoint(float key, float value)
{
}

void Idogep::ManageCurveRole::CommitRemovePoint(float key, float value)
{
}

#endif

