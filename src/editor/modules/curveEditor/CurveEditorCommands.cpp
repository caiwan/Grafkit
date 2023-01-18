#include "CurveEditorCommands.h"

#include "modules/curveEditor/CurvePointEditor.h"

using namespace Grafkit;
using namespace Idogep;

CurveKeyChangeCommand::CurveKeyChangeCommand(
    Ref<Animation::Channel>& channel, const size_t keyId,
    const Animation::Key oldKey, const Animation::Key newKey, Ref<CurvePointEditor> pointRole
)
    : m_channel(channel)
  , m_id(keyId)
  , m_newKey(newKey)
  , m_oldKey(oldKey)
  , m_pointEditor(pointRole)
{
	assert(m_pointEditor);
}

CurveKeyChangeCommand::~CurveKeyChangeCommand() = default;


void CurveKeyChangeCommand::Do()
{
    m_channel->SetKey(m_id, m_newKey);
    //m_pointEditor->Rebuild();
}

void CurveKeyChangeCommand::Undo()
{
    m_channel->SetKey(m_id, m_oldKey);
    //m_pointEditor->Rebuild();
    // what to do
}
