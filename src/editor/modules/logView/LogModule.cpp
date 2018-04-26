#include "LogModule.h"

#include "logwidget.h"

Idogep::LogModule::LogModule(Ref<Module> parent) : Module(parent), m_myView(nullptr)
{
}

Idogep::LogModule::~LogModule()
{
}

void Idogep::LogModule::Initialize()
{
	DEBUG_ASSERT(m_parent.Valid());
	DEBUG_ASSERT(m_parent->GetView().Valid());
	QWidget * parentWidget = dynamic_cast<QWidget*>(m_parent->GetView().Get());
	DEBUG_ASSERT();
	m_myView = new LogWidget(parentWidget);
	SetView(m_myView);
}

void Idogep::LogModule::MediateSiblingModule(Ref<Module> other)
{
	//...
}
