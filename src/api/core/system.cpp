#include "System.h"
#include "input.h"
#include "exceptions.h"

#include "easyloggingpp.h"

#define ELPP_THREAD_SAFE

#ifndef _DEBUG
#define ELPP_DISABLE_DEBUG_LOGS
#define ELPP_DISABLE_INFO_LOGS
#define ELPP_DISABLE_VERBOSE_LOGS
#define ELPP_DISABLE_TRACE_LOGS
#endif // _DEBUG

#include "easyloggingpp.h"

INITIALIZE_EASYLOGGINGPP


using namespace FWcore;

System::System()
	: Window::WindowHandler() , m_window(this), m_Input(nullptr)
{
	// init logger 
	el::Configurations defaultConf;
	defaultConf.setToDefault();

#ifdef _DEBUG
	defaultConf.setGlobally(el::ConfigurationType::Format, "%logger [%levshort]: %msg - at %fbase function %func line %line tid=%thread");
#else 
	defaultConf.setGlobally(el::ConfigurationType::Format, "[%levshort] %msg");
#endif // _DEBUG

	el::Loggers::addFlag(el::LoggingFlag::AutoSpacing);
	el::Loggers::reconfigureLogger("default", defaultConf);

	// LOG(TRACE) << SOMETHING;
}

System::~System()
{
}

int System::execute() {
	int result = 0;
	MSG msg;

	// Initialize the message structure.
	ZeroMemory(&msg, sizeof(MSG));
	
	// ================================================================================================================================
	// --- init
	
	// +++ crete graphics device here 
	
	do {
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
			//DebugBreak();
			MessageBoxA(NULL, ex.what(), "Exception", 0);
			//goto teardown;
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
		catch (FWdebug::Exception& ex)
		{
			///@todo handle exceptions here 
			//  DebugBreak();
			MessageBoxA(NULL, ex.what(), "Exception", 0);
			// goto teardown;
			break;
		}
		// ================================================================================================================================
		// --- teardown

	} while (0);
	teardown: {
		this->release();
		this->ShutdownWindows();
	}
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
			m_Input->KeyDown((unsigned int)wparam);
			return 0;
		}

		// Check if a key has been released on the keyboard.
		case WM_KEYUP:
		{
			// If a key is released then send it to the input object so it can unset the state for that key.
			m_Input->KeyUp((unsigned int)wparam);
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

	m_Input = new Input;
	if (!m_Input)
	{
		///@todo dobjon exceptiont
		return;
	}

	m_Input->Initialize();

}

void System::ShutdownWindows()
{
	this->m_window.destroyWindow();
	return;
}

