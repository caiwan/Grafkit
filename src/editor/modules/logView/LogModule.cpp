#include "LogModule.h"
#include "proxies/LoggerProxy.h"

using namespace Idogep;

LogView::LogView(): View() {
}

LogModule::LogModule(LoggerProxy* const & loggerProxy) : Controller()
    , m_myView(nullptr)
    , m_loggerProxy(loggerProxy) {
}

LogModule::~LogModule() {
}

void LogModule::Initialize(Grafkit::IResourceManager* const& resourceManager)
{
    m_myView = dynamic_cast<LogView*>(View::SafeGetView(resourceManager, "LogView").Get());
    assert(m_myView.Valid());
    m_loggerProxy->onUpdateLog += Delegate(m_myView.Get(), &LogView::UpdateLog);
}
