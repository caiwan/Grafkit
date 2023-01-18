#include "EditorView.h"

#include "utils/Command.h"

using namespace Idogep;

// ========================================================================================================

void Roles::ManageCommandStackRole::CommandStackChangedEvent(CommandStack* const & stack)
{
    ToggleRedo(stack->HasRedo());
    ToggleUndo(stack->HasUndo());
}

void Roles::ManageCommandStackRole::ConnectCommandStackEvents(CommandStack* const & stack) { stack->onCommandStackChanged += Delegate(this, &ManageCommandStackRole::CommandStackChangedEvent); }

// ========================================================================================================

EditorView::EditorView()
    : View()
  , ManageCommandStackRole()
  , ManageFileOperationsRole()
{
}

