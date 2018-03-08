#include "Command.h"

void Idogep::CommandStack::AddCommand(Ref<Command>& command)
{
	m_redoStack.clear();
	m_undoStack.push_back(command);

	if (m_undoStack.size() > 1000)
		m_undoStack.pop_back();

	onCommandStackChanged(this);
}

void Idogep::CommandStack::Redo()
{
	if (m_redoStack.empty())
		return;

	auto cmd = m_redoStack.front();
	cmd->Do();

	onCommandStackChanged(this);
}

void Idogep::CommandStack::Undo()
{
	if (m_undoStack.empty())
		return;

	auto cmd = m_undoStack.front();
	cmd->Undo();

	m_undoStack.pop_front();
	m_redoStack.push_back(cmd);

	onCommandStackChanged(this);
}
