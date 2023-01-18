#include <imgui.h>

#include "application.h"
#include "platform/gk_window.h"
// ---- dummy window ----- 

class DummyWnd : public UI::Window {
public:
	DummyWnd() {}
	~DummyWnd() {}

	void Render() 
	{
		bool what = true;

		ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiSetCond_FirstUseEver);
		ImGui::Begin("Another Window", &what);
		ImGui::Text("Hello");
		ImGui::End();
	}
};


// ---- application ----- 
Application::Application()
{
}

int Application::CreateUI()
{
	this->addWindow(new DummyWnd());
	
	return 0;
}

int Application::UpdateUI()
{
	return 0;
}
