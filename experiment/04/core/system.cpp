#include "System.h"
#include "input.h"

using namespace FWcore;

System::System()
	: Window::WindowHandler() , m_window(this)
{
	m_Input = 0;
}

System::~System()
{
	//this->Shutdown();
}

int System::execute() {
	int result = 0;
	MSG msg;

	// Initialize the message structure.
	ZeroMemory(&msg, sizeof(MSG));
	
	// ================================================================================================================================
	// --- init
	
	// +++ crete graphics device here 
	
	// + exception handling
	result = this->init();
	if (result != 0) {
		this->release();
		return 1;
	}

	// ================================================================================================================================
	// --- mainloop
	// + exception handling
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

	// ================================================================================================================================
	// --- teardown
	this->release();
	return 0;
}

#if 0
bool System::Initialize()
{
	int screenWidth, screenHeight;
	bool result;


	// Initialize the width and height of the screen to zero before sending the variables into the function.
	screenWidth = 800;
	screenHeight = 600;

	// Initialize the windows api.
	InitializeWindows(screenWidth, screenHeight);

	// Create the input object.  This object will be used to handle reading the keyboard input from the user.
	m_Input = new Input;
	if(!m_Input)
	{
		return false;
	}

	// Initialize the input object.
	m_Input->Initialize();

	// --- snip 

	// Create the graphics object.  This object will handle rendering all the graphics for this application.
	/*
	m_Graphics = new GraphicsClass;
	if(!m_Graphics)
	{
		return false;
	}

	// Initialize the graphics object.
	result = m_Graphics->Initialize(screenWidth, screenHeight, this->window.getHWnd());
	if(!result)
	{
		return false;
	}
	*/

	// --- snap

	return true;
}

void System::Shutdown()
{
	// Release the graphics object.
	/*
	if(m_Graphics)
	{
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = 0;
	}

	*/

	// Release the input object.
	if(m_Input)
	{
		delete m_Input;
		m_Input = 0;
	}

	// Shutdown the window.
	ShutdownWindows();
	
	return;
}

void System::Run()
{
	MSG msg;
	bool done, result;


	// Initialize the message structure.
	ZeroMemory(&msg, sizeof(MSG));
	
	// Loop until there is a quit message from the window or the user.
	done = false;
	while(!done)
	{
		// Handle the windows messages.
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// If windows signals to end the application then exit out.
		if(msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			// Otherwise do the frame processing.
			result = Frame();
			if(!result)
			{
				done = true;
			}
		}

	}

	return;
}

#endif

/*
bool System::Frame()
{
	bool result;

	// Check if the user pressed escape and wants to exit the application.
	if(m_Input->IsKeyDown(VK_ESCAPE))
	{
		return false;
	}

	// Do the frame processing for the graphics object.
	result = m_Graphics->Frame();
	if(!result)
	{
		return false;
	}

	return true;
}
*/

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

void System::InitializeWindows(int screenWidth, int screenHeight, int fullscreen)
{
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

	// Release the pointer to this class.
	//ApplicationHandle = NULL;

	return;
}

