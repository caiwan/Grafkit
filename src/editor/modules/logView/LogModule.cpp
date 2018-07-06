#include "LogModule.h"
#include "proxies/LoggerProxy.h"

using namespace Idogep;

LogView::LogView(const Ref<Controller>& controller): View() {
}

LogModule::LogModule(LoggerProxy* const & loggerProxy) : Controller()
    , m_myView(nullptr)
    , m_loggerProxy(loggerProxy) {
}

LogModule::~LogModule() {
}

void LogModule::Initialize(Grafkit::IResourceManager* const& resourceManager) 
{
#if 0
    assert(m_parent.Valid());
    assert(m_parent->GetView().Valid());
    QWidget * parentWidget = dynamic_cast<QWidget*>(m_parent->GetView().Get());
    assert(parentWidget);
    m_myView = new LogWidget(parentWidget);
    SetView(m_myView);

    assert(m_loggerProxy);
    m_loggerProxy->onUpdateLog += Delegate(m_myView.Get(), &LogView::UpdateLog);

#endif
}
