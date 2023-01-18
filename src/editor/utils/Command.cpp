#include <cassert>
#include "utils/Command.h"

void Idogep::CommandStack::ConnectEmitter(EmitsCommandRole * emitter)
{
	assert(emitter);
	emitter->onNewCommand += Delegate(this, &CommandStack::AddCommand);
}

void Idogep::CommandStack::ClearStack()
{
	while (!m_redoStack.empty()) m_redoStack.pop();
}

void Idogep::CommandStack::AddCommand(Ref<Command> command)  // NOLINT
{
	ClearStack();

	// TODO: exception handling && rollback if needed
	command->Do();

	m_undoStack.push(command);

	onCommandStackChanged(this);
}

void Idogep::CommandStack::Redo()
{
	if (m_redoStack.empty())
		return;

	auto cmd = m_redoStack.top();
	cmd->Do();

	m_redoStack.pop();
	m_undoStack.push(cmd);

	onCommandStackChanged(this);
}

void Idogep::CommandStack::Undo()
{
	if (m_undoStack.empty())
		return;

	auto cmd = m_undoStack.top();
	cmd->Undo();

	m_undoStack.pop();
	m_redoStack.push(cmd);

	onCommandStackChanged(this);
}
