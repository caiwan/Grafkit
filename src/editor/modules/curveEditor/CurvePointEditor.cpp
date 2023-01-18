#include "CurvePointEditor.h"

#include "utils/Command.h"
#include "CurveEditorCommands.h"

#include "curveeditorscene.h"
#include "curvepointitem.h"

using namespace Idogep;
using namespace Grafkit;


CurvePointEditor::CurvePointEditor(const Ref<Controller>& parent)
    : Controller(parent)
{
}

void CurvePointEditor::Rebuild()
{
    assert(m_channel);

    m_points.clear();

    const size_t keyCount = m_channel->GetKeyCount();
    for (size_t i = 0; i < keyCount; i++) {
        const auto key = m_channel->GetKey(i);
        CurvePointItem *point = new CurvePointItem(key, i);

        m_points.push_back(point);

        point->onStartEdit += Delegate(this, &CurvePointEditor::StartEditEvent);
        point->onCommitEdit += Delegate(this, &CurvePointEditor::CommitEditEvent);
        point->onEditKey += Delegate(this, &CurvePointEditor::EditKeyEvent);

        point->SetModule(this);
    }
}

void CurvePointEditor::HidePoints() {
    for (CurvePointItem* point : m_points)
    {
        point->hide();
    }
}

void CurvePointEditor::ShowPoints() {
    for (CurvePointItem* point : m_points)
    {
        point->show();
    }
}

void CurvePointEditor::Recalculate(TimelineArea* const area) const
{
    for (CurvePointItem* point : m_points)
    {
        point->RecalculatePosition(area);
    }
}

void CurvePointEditor::UpdateKey(const Animation::ChannelRef& channel, size_t id, const Animation::Key& key) 
{
    if (channel != m_channel)
        return;

    m_points.at(id)->SetKey(key);
    m_points.at(id)->RequestRefreshView(false);
}

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
    const Animation::Key key = item->GetKey();
    const Animation::Key originalKey = item->GetOriginalKey();

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
    item->SetKey(EditKey(item->GetId(), item->GetKey()));
}

Animation::Key CurvePointEditor::EditKey(size_t index, Animation::Key key) const
{
    const double delta = (1. / 120.);

    if (index > 0)
    {
        const auto prevKey = m_channel->GetKey(index - 1);
        if (key.m_time <= prevKey.m_time) { key.m_time = prevKey.m_time + delta; }
    }

    if (index < m_channel->GetKeyCount() - 1)
    {
        const auto nextKey = m_channel->GetKey(index + 1);
        if (key.m_time > nextKey.m_time) { key.m_time = nextKey.m_time + delta; }
    }

    if (key.m_time < 0.)
        key.m_time = 0;

    m_channel->SetKey(index, key);
    return key;
}
