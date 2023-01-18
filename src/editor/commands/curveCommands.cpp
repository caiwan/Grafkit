#include "curveCommands.h"

#include "utils/ViewModule.h"

using namespace Grafkit;
using namespace Idogep;

CurveKeyChangeCommand::CurveKeyChangeCommand(
    Ref<Animation::Channel>& channel, const size_t keyId,
    const Animation::Key oldKey, const Animation::Key newKey, View* const & enclosingView
)
    : m_channel(channel)
  , m_id(keyId)
  , m_newKey(newKey)
  , m_oldKey(oldKey)
  , m_enclosingView(enclosingView)
{
	assert(m_enclosingView);
}

void CurveKeyChangeCommand::Do()
{
    m_channel->SetKey(m_id, m_newKey);
    m_enclosingView->RequestRefreshView(false);
}

void CurveKeyChangeCommand::Undo()
{
    m_channel->SetKey(m_id, m_oldKey);
    m_enclosingView->RequestRefreshView(false);
}
