//#include <qtimer.h>

#include "ViewModule.h"

using namespace Idogep;


Idogep::Module::Module() : m_view(nullptr), m_parent(nullptr)
{
}

Idogep::Module::~Module()
{
}

// ------------------------------------------------------------------------

Idogep::View::View()
{
	m_refreshQueueObject = new Roles::ViewRefreshQueue(this);
}

Idogep::View::~View()
{
	delete m_refreshQueueObject;
}

void Idogep::View::RequestRefreshView(bool force)
{
	// Request immediately
	if (force) {
		RefreshView(true);
	}
	else {
		//QTimer. ... 
		RefreshView(false);
	}
}

// ----------------------------------------------------

void Idogep::Roles::ViewRefreshQueue::refreshViewSlot()
{
	//m_view->RefreshView(false);
}

