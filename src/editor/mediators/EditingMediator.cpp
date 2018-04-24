#include "EditingMediator.h"

#include "models/ViewModule.h"
#include "modules/outlineView/scenegraphviewwidget.h"
#include "modules/curveEditor/curveeditorwidget.h"

using namespace Idogep;

Idogep::Roles::MediateOutlineViewRole::MediateOutlineViewRole()
{
}

void Idogep::Roles::MediateOutlineViewRole::MediateViews(View * parent, View * child)
{
	SceneGraphViewWidget* outlineViewer = dynamic_cast<SceneGraphViewWidget*>(parent);
	CurveEditorWidget * curveEditor = dynamic_cast<CurveEditorWidget*>(child);
	DEBUG_ASSERT(outlineViewer);
	DEBUG_ASSERT(curveEditor);
	outlineViewer->onItemSelected += Delegate(this, &Roles::MediateOutlineViewRole::ItemSelectedEvent);
	onAnimationSelected += Delegate(curveEditor, &CurveEditorWidget::AnimationChangedEvent);
}
void Idogep::Roles::MediateOutlineViewRole::ItemSelectedEvent(TreeItem * item)
{
}

// ========================================================================================================

#include "utils/Command.h"

Idogep::Roles::ManageCommandStackRole::ManageCommandStackRole() : m_redoAct(nullptr), m_undoAct(nullptr)
{
}

void Idogep::Roles::ManageCommandStackRole::CommandStackChangedEvent(CommandStack * const & stack)
{
	ToggleRedo(stack->HasRedo());
	ToggleUndo(stack->HasUndo());
}

void Idogep::Roles::ManageCommandStackRole::ConnectCommandStackEvents(CommandStack * const & stack)
{
	stack->onCommandStackChanged += Delegate(this, &ManageCommandStackRole::CommandStackChangedEvent);
}

