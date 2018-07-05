/**
    This file contains a base class to create and handle windows on the windows platform
*/

#pragma once

#ifndef NOMINAMX
#define NOMINMAX
#endif //NOMINMAX

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

/**
    Base window class
*/
class Window
{
public:
    class WindowHandler
    {
    public:
        virtual LRESULT MessageHandler(HWND, UINT, WPARAM, LPARAM) = 0;
    };

    Window(WindowHandler* handler, TCHAR* title);
    explicit Window(WindowHandler* handler);

private:
    /// Common constructor
    void init();

public:
    ~Window();

    /// Opens a window with the given specs
    void createWindow(int sx, int sy, int isfullscreen);

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
    int getWidth() const { return this->w_width; }
    int getHeight() const { return this->w_height; }
    int getRealWidth() const { return this->r_width; }
    int getRealHeight() const { return this->r_height; }

    bool getIsFullscreen() const { return this->fullscreen; }
    bool getIsShown() const { return this->isWindShown; }

    HWND getHWnd() const { return this->hWnd; }
    HDC getHDc() const { return this->hDc; }
    HGLRC getHRc() const { return this->hRc; }

    WindowHandler* getHandler() const { return this->handler; }

private:
    WindowHandler* handler;

    CHAR wndtitle[512];

    int w_height, w_width;
    int r_height, r_width;
    bool fullscreen;
    //int samplesPerPixel;

    bool isWindShown;

    HWND hWnd;
    HDC hDc;
    HGLRC hRc;

    void destructEverything();
};
