#include "View.h"
using namespace Idogep;

Idogep::View::View()
{
	m_refreshQueueObject = new ViewRefreshQueue(this);
}

Idogep::View::~View()
{
	delete m_refreshQueueObject;
}

void Idogep::View::RequestRefreshView(bool force)
{
	// Request immediately
	if (force) {
		RefreshView(force);
	}
	else {
		// TODO: enclose it into a timer;
		// to call it later 
		RefreshView(force);
	}
}
#include <qobject.h>

void Idogep::ViewRefreshQueue::refreshViewSlot()
{
}
