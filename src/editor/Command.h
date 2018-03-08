#pragma once 

#include <list>

#include "utils/reference.h"

#include "Event.h"

namespace Idogep{

	class CommandStack;

	class Command : public Referencable
	{
		friend class CommandStack;
	public :
	enum CommandState {
		DONE, NOT_DONE
	};

	public:
		Command(){}
		virtual ~Command(){}

		virtual void Do() = 0;
		virtual void Undo() = 0;

	private:
		CommandState m_status;
	};

	class CommandStack {
	public:
		CommandStack(){}
		CommandStack(){}

		void AddCommand(Ref<Command> &command);
		void Redo();
		void Undo();

		bool HasUndo() { return m_undoStack.empty(); }
		bool HasRedo() { return m_redoStack.empty(); }

		Event<CommandStack * const &> onCommandStackChanged;

	private:
		std::list<Ref<Command>> m_undoStack;
		std::list<Ref<Command>> m_redoStack;
	};

}
