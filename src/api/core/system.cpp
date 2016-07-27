#include "../stdafx.h"

#include "System.h"
#include "input.h"
#include "livereload.h"

///@todo add log mock

using namespace Grafkit;
using namespace FWdebugExceptions;

System::System()
	: Window::WindowHandler() , m_window(this), m_pInput(nullptr)
{
	// init logger 
	LOGGER(Log::Logger().Info("---- APPSTART ----"));
}

System::~System()
{
	delete m_pInput;

}

int System::execute() {
	int result = 0;
	MSG msg;

	// Initialize the message structure.
	ZeroMemory(&msg, sizeof(MSG));
	
	// ================================================================================================================================
	// --- init
	
	// +++ crete graphics device here 
#ifndef LIVE_RELEASE
	bool reload = false;
	do {
		reload = false;
#endif //LIVE_RELEASE
		// + exception handling
		try
		{
			result = this->init();
			if (result != 0) {
				this->release();
				return 1;
			}
		}
		catch (FWdebug::Exception& ex)
		{
			///@todo handle exceptions here 
			MessageBoxA(NULL, ex.what(), "Exception", 0);
			LOGGER(Log::Logger().Error(ex.what()));

			break;
		}

		// ================================================================================================================================
		// --- mainloop

		try
		{
			int done = 0;
			while (!done)
			{
				// Handle the windows messages.
				if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}

				// If windows signals to end the application then exit out.
				if (msg.message == WM_QUIT)
				{
					done = 1;
				}
				else
				{
					// Otherwise do the frame processing.
					result = this->mainloop();
					if (result != 0)
					{
						done = 1;
					}
				}

			}

		}
#ifndef LIVE_RELEASE
		catch (LiveReloadCannotReloadItem &ex) {
			LOGGER(Log::Logger().Error(ex.what()));
			reload = true;
		}
#endif //LIVE_RELEASE
		catch (FWdebug::Exception& ex)
		{
			///@todo handle exceptions here 
			//  DebugBreak();
			MessageBoxA(NULL, ex.what(), "Exception", 0);
			LOGGER(Log::Logger().Error(ex.what()));

			break;
		}
		// ================================================================================================================================
		// --- teardown
		{
			this->release();
		}

#ifndef LIVE_RELEASE
	} while (reload);
#endif //LIVE_RELEASE

	this->ShutdownWindows();

	return 0;
}



LRESULT CALLBACK System::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
	{
		switch (umsg)
		{
			// Check if a key has been pressed on the keyboard.
		case WM_KEYDOWN:
		{
			// If a key is pressed send it to the input object so it can record that state.
			m_pInput->KeyDown((unsigned int)wparam);
			return 0;
		}

		// Check if a key has been released on the keyboard.
		case WM_KEYUP:
		{
			// If a key is released then send it to the input object so it can unset the state for that key.
			m_pInput->KeyUp((unsigned int)wparam);
			return 0;
		}

		// Any other messages send to the default message handler as our application won't make use of them.
		default:
		{
			return DefWindowProc(hwnd, umsg, wparam, lparam);
		}
		}
	}

void System::InitializeWindows(int screenWidth, int screenHeight, int fullscreen, int resizeable)
{
	///@todo resizeable window

	this->m_window.createWindow(screenWidth, screenHeight, fullscreen);
	this->m_window.showWindow();

	m_pInput = new Input();
	m_pInput->Initialize();

}

void System::ShutdownWindows()
{
	this->m_window.destroyWindow();
	return;
}

