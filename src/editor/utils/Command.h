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

    typedef Ref<Command> CommandRef;

    class EmitsCommandRole
    {
    public:
        virtual ~EmitsCommandRole() = default;
        Event<CommandRef> onNewCommand;
    };

    class CommandStack
    {
    public:
        CommandStack() = default;

        void ConnectEmitter(EmitsCommandRole* emitter);

        void AddCommand(CommandRef command);
        void Redo();
        void Undo();

        bool HasUndo() const { return !m_undoStack.empty(); }
        bool HasRedo() const { return !m_redoStack.empty(); }

        void ClearStack();

        Event<CommandStack * const &> onCommandStackChanged;
        Event<CommandRef> onNewCommand;

    private:
        std::stack<CommandRef> m_undoStack;
        std::stack<CommandRef> m_redoStack;
    };
}
