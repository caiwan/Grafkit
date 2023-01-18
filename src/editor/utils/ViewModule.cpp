//#include <qtimer.h>

#include "ViewModule.h"

using namespace Idogep;

Module::Module(Ref<Module> parent)
    : Referencable()
{
    if (parent.Valid())
        parent->AddChildModule(this);
    m_parent = parent;
}

Module::~Module()
{
}

// ------------------------------------------------------------------------

View::View(Ref<Module> parentModule)
    : m_module(parentModule) { m_refreshQueueObject = new Roles::ViewRefreshQueue(this); }

View::~View() { delete m_refreshQueueObject; }

void View::RequestRefreshView(const bool force)
{
    // Request immediately
    if (force) { RefreshView(true); }
    else
    {
        //QTimer. ... 
        RefreshView(false);
    }
}


// ----------------------------------------------------

void Roles::ViewRefreshQueue::refreshViewSlot() const { m_view->RefreshView(false); }
