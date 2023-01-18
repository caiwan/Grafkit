#include "platform/gk_imgui_app.h"

/// ... implement 

class Application : public ApplicationInterface 
{
	public:
		static Application& Instance(){
			static Application app;
			return app;
		}

	private:
		Application();

	protected:
		virtual int CreateUI();
		virtual int UpdateUI();
};
