#pragma once 

#include <list>

#include "utils/reference.h"

#include "utils/Event.h"

namespace Idogep {

	class CommandStack;

	class Command : public Referencable
	{
		friend class CommandStack;
	public:
		enum CommandState {
			DONE, NOT_DONE
		};

	public:
		Command() {}
		virtual ~Command() {}

		virtual void Do() = 0;
		virtual void Undo() = 0;

	private:
		CommandState m_status;
	};

	class EmitsCommandRole {
	public:
		Event<Ref<Command>> onNewCommand;
	};

	class CommandStack {
	public:
		CommandStack() {}

		void ConnectEmitter(EmitsCommandRole* emitter);

		void AddCommand(Ref<Command> command);
		void Redo();
		void Undo();

		bool HasUndo() { return m_undoStack.empty(); }
		bool HasRedo() { return m_redoStack.empty(); }

		Event<CommandStack * const &> onCommandStackChanged;
		Event<Ref<Command>> onNewCommand;

	private:
		std::list<Ref<Command>> m_undoStack;
		std::list<Ref<Command>> m_redoStack;
	};


}
