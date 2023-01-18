#include "CurveEditorCommands.h"

#include "modules/curveEditor/CurvePointEditor.h"

using namespace Grafkit;
using namespace Idogep;


CurveKeyChangeCommand::CurveKeyChangeCommand(Animation::ChannelRef& channel, size_t keyId, Animation::Key oldKey, Animation::Key newKey, CurvePointEditor* pointEditor): Command()
    , m_channel(channel)
    , m_id(keyId)
    , m_newKey(newKey)
    , m_oldKey(oldKey)
    , m_controller(pointEditor) {
}

CurveKeyChangeCommand::~CurveKeyChangeCommand() = default;


void CurveKeyChangeCommand::Do()
{
    m_channel->SetKey(m_id, m_newKey);
    m_controller->UpdateKey(m_channel, m_id, m_newKey);
}

void CurveKeyChangeCommand::Undo()
{
    m_channel->SetKey(m_id, m_oldKey);
    m_controller->UpdateKey(m_channel, m_id, m_oldKey);
}


CurveChangeCommand::CurveChangeCommand(const Animation::TrackRef& track, size_t id, const Animation::ChannelRef& oldChannel, const Animation::ChannelRef& newChannel, CurvePointEditor* controller): Command()
    , m_track(track)
    , m_id(id)
    , m_oldChannel(oldChannel)
    , m_newChannel(newChannel)
    , m_controller(controller) {
}

CurveChangeCommand::~CurveChangeCommand() = default;

void CurveChangeCommand::Do()
{
    m_track->SetChannel(m_id, m_newChannel);
    m_controller->UpdateChannel(m_track, m_id, m_newChannel);
}

void CurveChangeCommand::Undo()
{
    m_track->SetChannel(m_id, m_oldChannel);
    m_controller->UpdateChannel(m_track, m_id, m_newChannel);
}
