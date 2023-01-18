#include "curvedoc.h"

#include "utils/Command.h"
#include "commands/curveCommands.h"

#include "curveeditorscene.h"
#include "curvepointitem.h"

using namespace Idogep;

void Roles::EditCurveRole::StartEdit(CurvePointItem* item)
{
    // no need for this ATM
}

void Roles::EditCurveRole::CommitEdit(CurvePointItem* item)
{
    // emit command 
	const size_t index = item->GetId();
    const Grafkit::Animation::Key key = item->GetKey();
    const Grafkit::Animation::Key originalKey = item->GetOriginalKey();
	View* enclosingView = dynamic_cast<View*>(item->scene());
	CurveKeyChangeCommand *cmd = new CurveKeyChangeCommand(m_channel, index, originalKey, key, enclosingView);
	onNewCommand(cmd);
}

void Roles::EditCurveRole::CommitAddPoint(float key, float value)
{
    //recalc, readd? 
}

void Roles::EditCurveRole::CommitRemovePoint(float key, float value)
{
    //recalc, readd?
}

void Idogep::Roles::EditCurveRole::EditKey(CurvePointItem * item)
{
    const size_t index = item->GetId();
	m_channel->SetKey(index, item->GetKey());
}

// ========================================================================

void CurveManager::Rebuild()
{
	//delete m_curvePoints;
	if (m_curvePoints)
		m_curvePoints->clear();
	else
		m_curvePoints = new QList<Idogep::CurvePointItem*>();

    const size_t keyCount = m_channel->GetKeyCount();
	for (size_t i = 0; i < keyCount; i++) {
		const auto key = m_channel->GetKey(i);
		CurvePointItem *point = new CurvePointItem(key, i);
		m_curvePoints->push_back(point);

		point->onStartEdit += Delegate(this, &Roles::EditCurveRole::StartEdit);
		point->onCommitEdit += Delegate(this, &Roles::EditCurveRole::CommitEdit);
		point->onEditKey += Delegate(this, &Roles::EditCurveRole::EditKey);

	}
}

void CurveManager::AddCurveToScene(CurveSceneView* parent) const
{
	if (!m_curvePoints)
		return;

	for (size_t i = 0; i < m_curvePoints->size(); i++)
	{
		const auto point = m_curvePoints->at(i);
		parent->AddCurvePoint(point);
	}
	parent->RequestRefreshView(true);
}

void CurveManager::Recalculate(TimelineArea* const area) const
{
	for (size_t i = 0; i < m_curvePoints->size(); i++)
	{
		auto point = m_curvePoints->at(i);
		point->RecalculatePosition(area);
	}
}


#if 0
Idogep::EditCurveRole::EditCurveRole() :
	ManageCurveAudiogramRole(),
	m_curvePoints(nullptr)
{
}

void Idogep::EditCurveRole::SetChannel(Ref<Grafkit::Animation::Channel>& channel)
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

		point->onStartEdit += Delegate(this, &EditCurveRole::StartEdit);
		point->onCommitEdit += Delegate(this, &EditCurveRole::CommitEdit);
		point->onEditKey += Delegate(this, &EditCurveRole::MovePoint);
		point->onMoveTangent += Delegate(this, &EditCurveRole::MoveTangent);
	}

#endif 
}

void Idogep::EditCurveRole::Recalculate()
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

void Idogep::EditCurveRole::AddCurveToScene(CurveEditorScene * parent)
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

void Idogep::EditCurveRole::MovePoint(CurvePointItem * item)
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

void Idogep::EditCurveRole::MoveTangent(CurvePointItem * item)
{
	// ... 
}

void Idogep::EditCurveRole::StartEdit(CurvePointItem * item)
{
#if 0
	m_originalKey = item->GetKey();
#endif 0
}

void Idogep::EditCurveRole::CommitEdit(CurvePointItem * item)
{
#if 0
	m_modifiedKey = item->GetKey();
	CurveKeyChangeCommand * cmd = new CurveKeyChangeCommand(m_channel, item->index(), m_originalKey, m_modifiedKey, nullptr);
	//cmd->onRefreshView += Delegate(this, &EditCurveRole::RefreshView);
	onNewCommand(cmd);
#endif
}

void Idogep::EditCurveRole::CommitAddPoint(float key, float value)
{
}

void Idogep::EditCurveRole::CommitRemovePoint(float key, float value)
{
}

#endif

