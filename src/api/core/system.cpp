#include "stdafx.h"

#include "System.h"
#include "input.h"
#include "livereload.h"

#include "window.h"

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
		catch (FWdebug::Exception *& ex)
		{
			///@todo handle exceptions here 
			MessageBoxA(NULL, ex->what(), "Exception", 0);
			LOGGER(Log::Logger().Error(ex->what()));

			delete ex;

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



LRESULT System::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
	{
		switch (umsg)
		{
		case WM_KEYDOWN:
		{
			m_pInput->KeyDown((unsigned int)wparam);
			return 0;
		}

		case WM_KEYUP:
		{
			m_pInput->KeyUp((unsigned int)wparam);
			return 0;
		}
		default:
		{
			return DefWindowProc(hwnd, umsg, wparam, lparam);
		}
		}
	}

void System::InitializeWindows(int screenWidth, int screenHeight, int fullscreen, int resizeable)
{
	///@todo resizeable window

	//this->m_window = new Window(this);
	this->m_window.createWindow(screenWidth, screenHeight, fullscreen);
	this->m_window.showWindow();

	m_pInput = new Input();
	m_pInput->Initialize();

}

void System::ShutdownWindows()
{
	//if (m_window) {
		this->m_window.destroyWindow();
		//delete this->m_window;
		//this->m_window = NULL;
	//}
}

