//#include <qtimer.h>

#include "ViewModule.h"

using namespace Idogep;

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

void Idogep::Roles::ViewRefreshQueue::refreshViewSlot()
{
	//m_view->RefreshView(false);
}
