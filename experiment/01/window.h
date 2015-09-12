/**
	This file contains a base class to create and handle windows on the windows platform
*/

#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#undef NOMINMAX

/**
	Base window class
*/
class Window
{
	public:
		/// @todo window handler

	public:
		Window(TCHAR *title);
		Window();

	private:
		/// Common constructor
		void init();

	public:
		~Window();

		/// Opens a window with the given specs
		void createWindow(int sx, int sy, bool isfullscreen);

		/// Closes and releases device contexts
		void destroyWindow();

		/**
		@obsolete
		Closes, releases the device context and creates a new window with the prevoius settings.
		OpenGL miatt volt ra szukseg, hogy betoltsed az extensionoket.
		*/
		void recreateWindow();

		///Obvious
		void showWindow();
		///Obvious
		void hideWindow();

		/// @return true, if engine halt was requested.
		bool peekWindowMessage();

		//set
		void setTitle(const char* txt);

		//get
		int getWidth() {return this->w_width;}
		int getHeight() {return this->w_height;}
		int getRealWidth() {return this->r_width;}
		int getRealHeight() {return this->r_height;}

		bool getIsFullscreen() {return this->fullscreen;}
		bool getIsShown() {return this->isWindShown;}

		HWND getHWnd(){return this->hWnd;}
		HDC getHDc(){return this->hDc;}
		HGLRC getHRc() {return this->hRc;}

	private:
		static bool isMultisampleSupported;

		TCHAR wndtitle[512];

		int w_height, w_width;
		int r_height, r_width;
		bool fullscreen;
		int samplesPerPixel;

		bool isWindShown;

		HWND hWnd;
		HDC hDc;
		HGLRC hRc;

		void destructEverything();
};
