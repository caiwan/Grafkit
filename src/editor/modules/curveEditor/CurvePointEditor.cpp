#include "CurvePointEditor.h"

#include "utils/Command.h"
#include "CurveEditorCommands.h"

#include "curveeditorscene.h"
#include "curvepointitem.h"

using namespace Idogep;



CurvePointEditor::CurvePointEditor(const Ref<Controller>& parent)
	: Controller(parent)
{
}

void CurvePointEditor::Rebuild()
{
	m_curvePoints.clear();

	const size_t keyCount = m_channel->GetKeyCount();
	for (size_t i = 0; i < keyCount; i++) {
		const auto key = m_channel->GetKey(i);
		CurvePointItem *point = new CurvePointItem(key, i);
		m_curvePoints.push_back(point);

		point->onStartEdit += Delegate(this, &CurvePointEditor::StartEditEvent);
		point->onCommitEdit += Delegate(this, &CurvePointEditor::CommitEditEvent);
		point->onEditKey += Delegate(this, &CurvePointEditor::EditKeyEvent);

		point->SetModule(this);
	}
}

void CurvePointEditor::AddCurveToScene(CurveEditorView* parent) const
{
    // TODO: Move it to Curve Controller
	if (m_curvePoints.empty())
		return;

	for (Ref<CurvePointItem> point : m_curvePoints)
	{
		parent->AddCurvePoint(point);
	}
	parent->RequestRefreshView(true);
}

void CurvePointEditor::Recalculate(TimelineArea* const area) const
{
	for (Ref<CurvePointItem> point : m_curvePoints)
	{
		point->RecalculatePosition(area);
	}
}

//void Idogep::CurvePointEditor::RefreshPoint(ChannelRef channel, size_t id, AnimationKey key)
//{
//    if (channel != m_channel)
//        return;
//    
//    m_curvePoints.at(id)->SetKey(key);
//    m_curvePoints.at(id)->RequestRefreshView(false);
//}

void CurvePointEditor::Initialize()
{
	// ... 
}

// ========================================================================

void CurvePointEditor::StartEditEvent(CurvePointItem* item)
{
    // no need for this ATM
}

void CurvePointEditor::CommitEditEvent(CurvePointItem* item)
{
    // emit command 
    const size_t index = item->GetId();
    const Grafkit::Animation::Key key = item->GetKey();
    const Grafkit::Animation::Key originalKey = item->GetOriginalKey();

    CurveKeyChangeCommand *cmd = new CurveKeyChangeCommand(m_channel, index, originalKey, key, this);
    onNewCommand(cmd);
}

void CurvePointEditor::CommitAddPointEvent(float key, float value)
{
    //recalc, readd? 
}

void CurvePointEditor::CommitRemovePointEvent(float key, float value)
{
    //recalc, readd?
}

void CurvePointEditor::EditKeyEvent(CurvePointItem * item)
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