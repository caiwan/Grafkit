#include "LogModule.h"
#include "logwidget.h"
#include "proxies/LoggerProxy.h"

Idogep::LogModule::LogModule(Ref<Module> parent, LoggerProxy * const & loggerProxy) : Module(parent),
m_myView(nullptr), m_loggerProxy(loggerProxy)
{
}

Idogep::LogModule::~LogModule()
{
}

void Idogep::LogModule::Initialize()
{
	assert(m_parent.Valid());
	assert(m_parent->GetView().Valid());
	QWidget * parentWidget = dynamic_cast<QWidget*>(m_parent->GetView().Get());
	assert(parentWidget);
	m_myView = new LogWidget(parentWidget);
	SetView(m_myView);
	
	assert(m_loggerProxy);
	m_loggerProxy->onUpdateLog += Delegate(m_myView.Get(), &LogView::UpdateLog);
}

//void Idogep::LogModule::MediateSiblingModule(Ref<Module> other)
//{
//	//...
//}
