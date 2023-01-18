#pragma once 

#define WIN32_LEAN_AND_MEAN
#ifndef NOMINAMX
#define NOMINMAX
#endif //NOMINMAX
#include <windows.h>

#include "window.h"
#include "input.h"

namespace Grafkit 
{
	class Input;

	class System : public Window::WindowHandler
	{
	public:
		System();
		virtual ~System();

		int execute();

		virtual int init() = 0;
		virtual int mainloop() = 0;
		virtual void release() = 0;

		LRESULT MessageHandler(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) override;

	protected:
		void InitializeWindows(int sreenWidth = 800, int screenHeight = 600, int isFullscreen = 0, int resizeable = 0, const char *pTitle = "Grafkit2 Application");
		void ShutdownWindows();

	protected:
		Window m_window;
		Input* m_pInput;

		// shortcut for esc
		bool isEscPressed() const { return m_pInput->IsKeyDown(VK_ESCAPE); }

	};

}
