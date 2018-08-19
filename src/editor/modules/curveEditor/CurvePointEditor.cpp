#include "CurvePointEditor.h"

#include <QDebug>

#include "utils/Command.h"
#include "CurveEditorCommands.h"

#include "curveeditorscene.h"
#include "curvepointitem.h"

#include "AnimationModule.h"
#include <qmessagebox.h>

using namespace Idogep;
using namespace Grafkit;


CurvePointEditor::CurvePointEditor()
    : Controller()
    , m_channelId(0)
    , m_myView(nullptr) {
}

void CurvePointEditor::Initialize(IResourceManager* const& resourceManager)
{
    CurveEditorScene *ces = dynamic_cast<CurveEditorScene*>(View::SafeGetView(resourceManager, "CurveEditorView").Get());
    assert(ces);

    ces->onPointDeSelected += Delegate(this, &CurvePointEditor::PointDeSelectedEvent);
    ces->onPointSelected += Delegate(this, &CurvePointEditor::PointSelectedEvent);

    ces->onCommitAddPointEvent += Delegate(this, &CurvePointEditor::CommitAddPointEvent);
    ces->onCommitRemovePointEvent += Delegate(this, &CurvePointEditor::CommitRemovePointEvent);

    m_myView = dynamic_cast<PointEditorView*>(View::SafeGetView(resourceManager, "PointEditorView").Get());
    assert(m_myView);

    m_myView->onEditKey += Delegate(this, &CurvePointEditor::EditKeyEvent);
    m_myView->onStartEdit += Delegate(this, &CurvePointEditor::StartEditEvent);
    m_myView->onCommitEdit += Delegate(this, &CurvePointEditor::CommitEditEvent);
}

void CurvePointEditor::Rebuild()
{
    assert(m_channel);

    m_points.clear();

    const size_t keyCount = m_channel->GetKeyCount();
    for (size_t i = 0; i < keyCount; i++)
    {
        const auto key = m_channel->GetKey(i);
        CurvePointItem* point = new CurvePointItem(key, i);

        m_points.push_back(point);

        point->onStartEdit += Delegate(this, &CurvePointEditor::StartEditEvent);
        point->onCommitEdit += Delegate(this, &CurvePointEditor::CommitEditEvent);
        point->onEditKey += Delegate(this, &CurvePointEditor::EditKeyEvent);
    }

    onInvlaidateCurve();

}

void CurvePointEditor::HidePoints() { for (CurvePointItem* point : m_points) { point->hide(); } }

void CurvePointEditor::ShowPoints() { for (CurvePointItem* point : m_points) { point->show(); } }

void CurvePointEditor::Recalculate(TimelineArea* const &area) { for (CurvePointItem* point : m_points) { point->RecalculatePosition(area); } }

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

    onInvlaidateCurve();

}

void CurvePointEditor::UpdateChannel(const Animation::TrackRef & track, size_t channelId, const Animation::ChannelRef & channel)
{
    if (m_track != track || channelId != m_channelId)
        return;

    m_channel = channel;
    //m_channel->Clear();
    //channel->CopyTo(m_channel, 0, channel->GetKeyCount());
    
    Rebuild();
    onUpdateChannel();
}

// -------------------------------------------------------------------------------------------

void CurvePointEditor::StartEditEvent(const size_t& index, const Animation::Key& key)
{
    CurvePointItem* point = m_points[index];
    point->SetOriginalKey(key);

    onInvlaidateCurve();
}

void CurvePointEditor::CommitEditEvent(const size_t& index, const Animation::Key& key)
{
    CurvePointItem* point = m_points[index];
    const Animation::Key originalKey = point->GetOriginalKey();

    CurveKeyChangeCommand* cmd = new CurveKeyChangeCommand(m_channel, index, originalKey, key, this);
    onNewCommand(cmd);
    onInvlaidateCurve();
}

void CurvePointEditor::CommitAddPointEvent(const float& key, const float& value)
{
    Animation::ChannelRef oldChannel = new Animation::Channel(m_channel);

    int index = m_channel->FindKeyIndex(key);

    Animation::Key newKey(m_channel->GetKey(std::max(index,0))); // use previous key as prototype, or the preceedign if -1
    newKey.m_time = key, newKey.m_value = value;

    m_channel->InsertKey(index + 1 , newKey);

    Animation::ChannelRef newChannel = new Animation::Channel(m_channel);

    Ref<CurveChangeCommand> cmd = new CurveChangeCommand(m_track, m_channelId, oldChannel, newChannel, this);
    onNewCommand(cmd);
    Rebuild();
    onUpdateChannel();
}

void CurvePointEditor::CommitRemovePointEvent(const size_t& index)
{
    if (m_channel->GetKeyCount() == 1)
    {
        QMessageBox::critical(nullptr, "Az ellen nem ved", "You're about to remove the last control point. I'm here to prevent you from this terrible action.");
        return;
    }

    Animation::ChannelRef oldChannel = new Animation::Channel(m_channel);

    m_channel->DeleteKey(index);

    Animation::ChannelRef newChannel = new Animation::Channel(m_channel);
    Ref<CurveChangeCommand> cmd = new CurveChangeCommand(m_track, m_channelId, oldChannel, newChannel, this);
    onNewCommand(cmd);
    Rebuild();
    onUpdateChannel();
}

void CurvePointEditor::EditKeyEvent(const size_t& index, const Animation::Key& key)
{
    CurvePointItem* point = m_points[index];
    point->SetKey(EditKey(index, key));
    point->RequestRefreshView(false);

    PointSelectedEvent(index);
    m_myView->RequestRefreshView(false);
    onInvlaidateCurve();
}

void CurvePointEditor::PointSelectedEvent(size_t index)
{
    m_myView->SetPointId(index);
    m_myView->SetPointKey(m_channel->GetKey(index));
    m_myView->UpdatePointEditor(true);
}

void CurvePointEditor::PointDeSelectedEvent() { m_myView->UpdatePointEditor(false); }


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

// ===========================================================================================

PointEditorView::PointEditorView() : View(), m_pointId(0) {
}
