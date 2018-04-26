#include "EditingMediator.h"

#if 0

#include "ViewModule.h"
#include "modules/outlineView/scenegraphviewwidget.h"
#include "modules/curveEditor/curveeditorwidget.h"
#include "OutlineViewMediator.h"

using namespace Idogep;

Idogep::Roles::MediateOutlineViewRole::MediateOutlineViewRole()
{
}

void Idogep::Roles::MediateOutlineViewRole::MediateViews(View * parent, View * child)
{
	SceneGraphViewWidget* outlineViewer = dynamic_cast<SceneGraphViewWidget*>(parent);
	DEBUG_ASSERT(outlineViewer);

	CurveEditorWidget * curveEditor = dynamic_cast<CurveEditorWidget*>(child);
	if (curveEditor) {
		onAnimationSelected += Delegate(curveEditor, &CurveEditorWidget::AnimationChangedEvent);
	}

	// ... 
}

void Idogep::Roles::MediateOutlineViewRole::MediateRoles(View * parent)
{
	SceneGraphViewWidget* outlineViewer = dynamic_cast<SceneGraphViewWidget*>(parent);
	DEBUG_ASSERT(outlineViewer);
	outlineViewer->onItemSelected += Delegate(this, &Roles::MediateOutlineViewRole::ItemSelectedEvent);
}

void Idogep::Roles::MediateOutlineViewRole::ItemSelectedEvent(TreeItem * item)
{
	// ... 
}

#endif
