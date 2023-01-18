//#include <qtimer.h>

#include "ViewModule.h"

using namespace Idogep;

Controller::Controller(Ref<Controller> parent)
	: Referencable()
{
	if (parent.Valid())
		parent->AddChildModule(this);
	m_parent = parent;
}

Controller::~Controller()
{
}

// ------------------------------------------------------------------------

View::View(Ref<Controller> parentModule)
	: m_module(parentModule)
    //, m_refreshQueueObject(nullptr)
{
	//m_refreshQueueObject = new Roles::ViewRefreshQueue(this);
}

View::~View() { /*delete m_refreshQueueObject;*/ }

void View::RequestRefreshView(const bool force)
{
	// Request immediately
	if (force)
	{
		RefreshView(true);
	}
	else
	{
		//QTimer. ... 
		RefreshView(false);
	}
}


// ----------------------------------------------------

void Roles::ViewRefreshQueue::refreshViewSlot() const { m_view->RefreshView(false); }
