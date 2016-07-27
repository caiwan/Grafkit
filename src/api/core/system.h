/**
*/

#pragma once 

#define WIN32_LEAN_AND_MEAN

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


		LRESULT CALLBACK MessageHandler(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

	protected:
		void InitializeWindows(int sreenWidth = 800, int screenHeight = 600, int isFullscreen = 0, int resizeable = 0);
		void ShutdownWindows();

	protected:
		Window m_window;
		Input* m_pInput;
	};

}


