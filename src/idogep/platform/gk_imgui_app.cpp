#include "gk_imgui_app.h"

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
	
	// -- create render preview window, and keep track of it 

	CreateUI();

	return 0;
}

int ApplicationInterface::mainloop()
{
	FWrender::Renderer &renderer = RendererInstance();

	renderer.BeginScene();
	
	// ... 

	renderer.EndScene();

	UpdateUI();
	return 0;
}

void ApplicationInterface::release()
{
}

LRESULT ApplicationInterface::MessageHandler(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	return LRESULT();
}
