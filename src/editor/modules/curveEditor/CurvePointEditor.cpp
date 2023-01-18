#include "CurvePointEditor.h"

#include "utils/Command.h"
#include "CurveEditorCommands.h"

#include "curveeditorscene.h"
#include "curvepointitem.h"

#include "AnimationView.h"

using namespace Idogep;
using namespace Grafkit;


CurvePointEditor::CurvePointEditor(const Ref<Controller>& parent)
    : Controller(parent)
    , m_myView(nullptr) 
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

void CurvePointEditor::UpdateKey(const Animation::ChannelRef& channel, size_t index, const Animation::Key& key) 
{
    if (channel != m_channel)
        return;

    m_points.at(index)->SetKey(key);
    m_points.at(index)->RequestRefreshView(false);
    m_myView->RequestRefreshView(false);

    if (index == m_myView->GetPointId())
    {
        PointSelectedEvent(index);

    }
}

void CurvePointEditor::Initialize(PointEditorView* pointEditorView) {
    m_myView = pointEditorView;
}


void CurvePointEditor::Initialize()
{
    assert(m_myView);
    m_myView->onEditKey += Delegate(this, &CurvePointEditor::EditKeyEvent);
    m_myView->onStartEdit+= Delegate(this, &CurvePointEditor::StartEditEvent);
    m_myView->onCommitEdit+= Delegate(this, &CurvePointEditor::CommitEditEvent);
}

// ========================================================================

void CurvePointEditor::StartEditEvent(const size_t& index, const Grafkit::Animation::Key& key)
{
    CurvePointItem* point = m_points[index];
    point->SetOriginalKey(key);
}

void CurvePointEditor::CommitEditEvent(const size_t& index, const Grafkit::Animation::Key& key)
{
    CurvePointItem* point = m_points[index];
    const Animation::Key originalKey = point->GetOriginalKey();

    CurveKeyChangeCommand *cmd = new CurveKeyChangeCommand(m_channel, index, originalKey, key, this);
    onNewCommand(cmd);
}

void CurvePointEditor::CommitAddPointEvent(const float& key, const float& value)
{
    //recalc, readd? 
    assert(0);
}

void CurvePointEditor::CommitRemovePointEvent(const float& key, const float& value)
{
    //recalc, readd?
    assert(0);
}

void CurvePointEditor::EditKeyEvent(const size_t& index, const Grafkit::Animation::Key& key) {
    CurvePointItem* point = m_points[index];
    point->SetKey(EditKey(index, key));
    point->RequestRefreshView(false);
    
    PointSelectedEvent(index);
    m_myView->RequestRefreshView(false);
}

void CurvePointEditor::PointSelectedEvent(size_t index) {

    m_myView->SetPointId(index);
    m_myView->SetPointKey(m_channel->GetKey(index));
    m_myView->UpdatePointEditor(true);
}

void CurvePointEditor::PointDeSelectedEvent() {
    m_myView->UpdatePointEditor(false);
}

Animation::Key CurvePointEditor::EditKey(size_t index, Animation::Key key) const
{
    const double delta = (1. / 120.);

    // time constraits
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

    // angle constraints

    const float angleEpsilon = 0.f;
    if (key.m_angle < -1. + angleEpsilon)
        key.m_angle = -1 + angleEpsilon;
    else if (key.m_angle > 1 - angleEpsilon)
        key.m_angle = 1 - angleEpsilon;

    // radius constraints 
    key.m_radius = std::max(key.m_radius, 0.01f);

    m_channel->SetKey(index, key);
    return key;
}

