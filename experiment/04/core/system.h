/**
	@brief
*/

#ifndef _System_H_
#define _System_H_

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#include "window.h"
#include "input.h"

namespace FWcore 
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

		/*
		bool Initialize();
		void Shutdown();
		void Run();
		*/

		LRESULT CALLBACK MessageHandler(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

	protected:
		//bool Frame();
		void InitializeWindows(int sreenWidth = 800, int screenHeight = 600, int isFullscreen = 0);
		void ShutdownWindows();

	protected:
		Window m_window;
		Input* m_Input;
	};

	//static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
	//static System* ApplicationHandle = 0;

}

#endif