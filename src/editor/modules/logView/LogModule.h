#pragma once 

#include "ViewModule.h"

namespace Idogep {
	class LogView;

	// TODO: bring our beloved loging proxy here 

	class LogView : public View {
	public:
		virtual void UpdateLog(std::string log) = 0;
		virtual void SetModel(Grafkit::IResource * modelResource) {};
	};


	class LogModule :public Module {
	public:
		LogModule(Ref<Module> parent);
		~LogModule();

		void Initialize();
		void MediateSiblingModule(Ref<Module> other);

		void UpdateLog(std::string log) { m_myView->UpdateLog(log); }

	private:
		Ref<LogView> m_myView;
	};

}