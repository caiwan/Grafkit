#include <QTimer>

#include <cassert>

#include "ViewModule.h"
#include "utils/ResourceManager.h"

using namespace Idogep;
using namespace Grafkit;

Ref<View> View::SafeGetView(IResourceManager* const& resourceManager, const std::string& name) {
    Ref<Resource<View>> controller = resourceManager->GetByName<Resource<View>>(name);
    assert(controller.Valid());
    assert(controller->Valid());
    return static_cast<Ref<View>>(*controller);
}

View::View() : m_refreshQueueObject(nullptr)
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

Ref<Controller> Controller::SafeGetController(IResourceManager* const& resourceManager, const std::string& name) {
    Ref<Resource<Controller>> controller = resourceManager->GetByName<Resource<Controller>>(name);
    assert(controller.Valid());
    assert(controller->Valid());
    return static_cast<Ref<Controller>>(*controller);
}

Controller::Controller() {
}

Controller::~Controller() {
}


// ----------------------------------------------------

Roles::ViewRefreshEvent::ViewRefreshEvent(View* const& view) : m_view(view)
, m_isQueued(0)
, m_isSetForce(false) {
}

void Roles::ViewRefreshEvent::refreshViewSlot()
{
    // keep it resistant to spam refreshes and porcess the very last one
    if (--m_isQueued > 1)
    {
        return;
    }

    m_view->RefreshView(m_isSetForce);
}

void Roles::ViewRefreshEvent::QueueRefresh(bool isForce)
{
    m_isSetForce = isForce;
    m_isQueued++;
    QTimer::singleShot(0, this, SLOT(refreshViewSlot()));
}
