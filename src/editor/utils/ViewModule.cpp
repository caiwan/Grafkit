#include <QTimer>

#include "ViewModule.h"

using namespace Idogep;

Controller::Controller(Ref<Controller> parent)
    : Referencable()
{
    if (parent.Valid())
        parent->AddChildModule(this);
    m_parent = parent;
}

Controller::~Controller() {
}

// ------------------------------------------------------------------------

View::View(Ref<Controller> parentModule)
    : m_module(parentModule)
    , m_refreshQueueObject(nullptr)
{
    m_refreshQueueObject = new Roles::ViewRefreshEvent(this);
}

View::~View()
{
    assert(m_refreshQueueObject);
    delete m_refreshQueueObject;
    m_refreshQueueObject = nullptr;
}

void View::RequestRefreshView(const bool force)
{
    // setup refresh at the end of the event queue though a QObject Proxy
    m_refreshQueueObject->QueueRefresh(true);
}


// ----------------------------------------------------

Roles::ViewRefreshEvent::ViewRefreshEvent(View* const& view): m_view(view)
    , m_isQueued(false)
    , m_isSetForce(false) {
}

void Roles::ViewRefreshEvent::refreshViewSlot()
{
    m_view->RefreshView(m_isSetForce);
    m_isQueued = false;
}

void Roles::ViewRefreshEvent::QueueRefresh(bool isForce)
{
    if (m_isQueued)
        return;
    m_isSetForce = isForce;
    m_isQueued = true; // prevent spam
    QTimer::singleShot(0, this, SLOT(refreshViewSlot()));
}
