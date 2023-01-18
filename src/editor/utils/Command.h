#pragma once

#include <stack>

#include "utils/reference.h"

#include "utils/Event.h"

namespace Idogep
{
    class CommandStack;

    class Command : public Referencable
    {
        friend class CommandStack;
    public:
        enum CommandState
        {
            DONE
          , NOT_DONE
        };

        Command()
            : m_status(NOT_DONE)
        {
        }

        virtual ~Command() = default;

        virtual void Do() = 0;
        virtual void Undo() = 0;

        CommandState GetStatus() const { return m_status; }

    private:
        CommandState m_status;
    };

    class EmitsCommandRole
    {
    public:
        virtual ~EmitsCommandRole() = default;
        Event<Ref<Command>> onNewCommand;
    };

    class CommandStack
    {
    public:
        CommandStack() = default;

        void ConnectEmitter(EmitsCommandRole* emitter);

        void AddCommand(Ref<Command> command);
        void Redo();
        void Undo();

        bool HasUndo() const { return !m_undoStack.empty(); }
        bool HasRedo() const { return !m_redoStack.empty(); }

        Event<CommandStack * const &> onCommandStackChanged;
        Event<Ref<Command>> onNewCommand;

    private:
        void ClearStack();
        std::stack<Ref<Command>> m_undoStack;
        std::stack<Ref<Command>> m_redoStack;
    };
}
