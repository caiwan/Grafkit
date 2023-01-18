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

	CurveKeyChangeCommand *cmd = new CurveKeyChangeCommand(m_channel, index, originalKey, key, item);
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

	auto key = item->GetKey();
	const double delta = (1. / 120.);

	if (index > 0) {
		const auto prevKey = m_channel->GetKey(index - 1);
		if (key.m_time <= prevKey.m_time)
		{
			key.m_time = prevKey.m_time + delta;
		}
	}

	if (index < m_channel->GetKeyCount() - 1) {
		const auto nextKey = m_channel->GetKey(index + 1);
		if (key.m_time > nextKey.m_time)
		{
			key.m_time = nextKey.m_time + delta;
		}
	}

	if (key.m_time < 0.)
		key.m_time = 0;

	item->SetKey(key);
	m_channel->SetKey(index, key);
}

// ========================================================================

CurveManager::CurveManager(const Ref<Module>& parent)
	: Module(parent)
{
}

void CurveManager::Rebuild()
{
	m_curvePoints.clear();

	const size_t keyCount = m_channel->GetKeyCount();
	for (size_t i = 0; i < keyCount; i++) {
		const auto key = m_channel->GetKey(i);
		CurvePointItem *point = new CurvePointItem(key, i);
		m_curvePoints.push_back(point);

		point->onStartEdit += Delegate(this, &Roles::EditCurveRole::StartEdit);
		point->onCommitEdit += Delegate(this, &Roles::EditCurveRole::CommitEdit);
		point->onEditKey += Delegate(this, &Roles::EditCurveRole::EditKey);

		point->SetModule(this);
	}
}

void CurveManager::AddCurveToScene(CurveSceneView* parent) const
{
	if (m_curvePoints.empty())
		return;

	for (Ref<CurvePointItem> point : m_curvePoints)
	{
		parent->AddCurvePoint(point);
	}
	parent->RequestRefreshView(true);
}

void CurveManager::Recalculate(TimelineArea* const area) const
{
	for (Ref<CurvePointItem> point : m_curvePoints)
	{
		point->RecalculatePosition(area);
	}
}

void CurveManager::Initialize()
{
	// ... 
}

