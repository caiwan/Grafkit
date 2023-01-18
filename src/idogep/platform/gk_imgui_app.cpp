#include "gk_imgui_app.h"
#include <imgui.h>
#include "imgui_impl_dx11.h"

ApplicationInterface::ApplicationInterface()
	:
	m_screenWidth(800),
	m_screenHeight(600)
{
	// ... 
}

int ApplicationInterface::init()
{
	// settingswindow (?)
	int fullscreen = 0;
	// load settings (?)

	InitializeWindows(m_screenWidth, m_screenHeight, fullscreen, 1);
	FWrender::Renderer &renderer = RendererInstance();
	renderer.Initialize(m_screenWidth, m_screenHeight, false, this->m_window.getHWnd(), false);

	// -- init imgui
	ImGui_ImplDX11_Init(this->m_window.getHWnd(), renderer.GetDevice(), renderer.GetDeviceContext());
	
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontDefault();

	// -- create render preview window, and keep track of it 
	CreateUI();

	return 0;
}

int ApplicationInterface::mainloop()
{
	UpdateUI();
	
	FWrender::Renderer &renderer = RendererInstance();

	ImGui_ImplDX11_NewFrame();
	
	for (size_t i = 0; i < this->m_windows.size(); i++) {
		UI::Window* wnd = this->m_windows[i];
		if (wnd) {
			wnd->Render();
		}
	}

	renderer.BeginScene(1, 1, 0, 1);
	ImGui::Render();
	renderer.EndScene();

	return 0;
}

void ApplicationInterface::release()
{
}

LRESULT ApplicationInterface::MessageHandler(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	return LRESULT();
}
