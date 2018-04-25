#include "EditingMediator.h"
#include "ViewModule.h"
#include "utils/Command.h"

using namespace Idogep;

// ========================================================================================================

Idogep::Roles::ManageCommandStackRole::ManageCommandStackRole()
{
}

void Idogep::Roles::ManageCommandStackRole::CommandStackChangedEvent(Idogep::CommandStack * const & stack)
{
	ToggleRedo(stack->HasRedo());
	ToggleUndo(stack->HasUndo());
}

void Idogep::Roles::ManageCommandStackRole::ConnectCommandStackEvents(Idogep::CommandStack * const & stack)
{
	stack->onCommandStackChanged += Delegate(this, &ManageCommandStackRole::CommandStackChangedEvent);
}

Idogep::EditorView::EditorView() : View(), Roles::ManageCommandStackRole(), Roles::ManageFileOperationsRole()
{
}
