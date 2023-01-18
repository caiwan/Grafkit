#include "curveCommands.h"

#include "utils/ViewModule.h"
#include "modules/curveEditor/curvepointitem.h"

using namespace Grafkit;
using namespace Idogep;

CurveKeyChangeCommand::CurveKeyChangeCommand(
    Ref<Animation::Channel>& channel, const size_t keyId,
    const Animation::Key oldKey, const Animation::Key newKey, Ref<CurvePointItem> point
)
    : m_channel(channel)
  , m_id(keyId)
  , m_newKey(newKey)
  , m_oldKey(oldKey)
  , m_pointItem(point)
{
	assert(m_pointItem);
}

CurveKeyChangeCommand::~CurveKeyChangeCommand() = default;


void CurveKeyChangeCommand::Do()
{
    m_channel->SetKey(m_id, m_newKey);
	m_pointItem->SetKey(m_newKey);
    m_pointItem->RequestRefreshView(false);
}

void CurveKeyChangeCommand::Undo()
{
    m_channel->SetKey(m_id, m_oldKey);
	m_pointItem->SetKey(m_oldKey);
    m_pointItem->RequestRefreshView(false);
}
