#include "curveCommands.h"

#include "utils/ViewModule.h"

using namespace Grafkit;
using namespace Idogep;

Idogep::CurveKeyChangeCommand::CurveKeyChangeCommand(Ref<Grafkit::Animation::Channel>& channel, size_t keyId,
	Grafkit::Animation::Key oldKey, Grafkit::Animation::Key newKey, View * const & enclosingView
)
	: m_channel(channel), m_id(keyId), m_newKey(newKey), m_oldKey(oldKey), m_enclosingView(enclosingView)
{
}

void Idogep::CurveKeyChangeCommand::Do()
{
	m_channel->SetKey(m_id, m_newKey);
	m_enclosingView->RequestRefreshView(false);
}

void Idogep::CurveKeyChangeCommand::Undo()
{
	m_channel->SetKey(m_id, m_oldKey);
	m_enclosingView->RequestRefreshView(false);
}

