#pragma once 

#include "utils/ViewModule.h"


namespace Idogep {
	class LogView;


	class LogView : public View {
	public:
		virtual void UpdateLog(std::string log) = 0;
		virtual void SetModel(Grafkit::IResource * modelResource) {};
	};

	class LoggerProxy;

	class LogModule : public Module {
	public:
		LogModule(Ref<Module> parent, LoggerProxy* const & loggerProxy);
		~LogModule();

		void Initialize();
		//void MediateSiblingModule(Ref<Module> other);

	private:
		Ref<LogView> m_myView;
		LoggerProxy* m_loggerProxy;
	};

}