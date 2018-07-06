#include <QTimer>

#include "ViewModule.h"
#include "utils/ResourceManager.h"

using namespace Idogep;
using namespace Grafkit;

View* View::SafeGetView(IResourceManager* const& resourceManager, const std::string& name) {
    Ref<Resource<View>> controller = resourceManager->Get<Resource<View>>(name);
    assert(controller.Valid());
    assert(controller->Valid());
    return controller->Get();
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

Controller* Controller::SafeGetController(IResourceManager* const& resourceManager, const std::string& name) {
    Ref<Resource<Controller>> controller = resourceManager->Get<Resource<Controller>>(name);
    assert(controller.Valid());
    assert(controller->Valid());
    return controller->Get();
}

Controller::Controller() {
}

Controller::~Controller() {
}


// ----------------------------------------------------

Roles::ViewRefreshEvent::ViewRefreshEvent(View* const& view) : m_view(view)
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
