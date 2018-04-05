#include "curveCommands.h"

using namespace Grafkit;
using namespace Idogep;

Idogep::CurveKeyChangeCommand::CurveKeyChangeCommand(Ref<Grafkit::Animation::Channel>& channel, size_t keyId, 
	Grafkit::Animation::Key oldKey,  Grafkit::Animation::Key newKey
)
	: m_channel(channel), m_id(keyId), m_newKey(newKey), m_oldKey(oldKey)
{
}

void Idogep::CurveKeyChangeCommand::Do()
{
	m_channel->SetKey(m_id, m_newKey);
	onRefreshView(false);
}

void Idogep::CurveKeyChangeCommand::Undo()
{
	m_channel->SetKey(m_id, m_oldKey);
	onRefreshView(false);
}

