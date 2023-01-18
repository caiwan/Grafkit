/**
	Platform extenson to integrate GK SDK and Imgui
*/

#pragma once 

#include "core/system.h"
#include "core/window.h"
#include "render/renderer.h"
#include "gk_window.h"

#include <vector>

class ApplicationInterface : public FWcore::System
{
	public:
		ApplicationInterface();

		/** singleton renderer instance */
		FWrender::Renderer& RendererInstance() {
			static FWrender::Renderer renderer;
			return renderer;
		}

	protected:
		virtual int CreateUI() = 0;
		virtual int UpdateUI() = 0;

	public:
		void addWindow(UI::Window* wnd) {
			this->m_windows.push_back(wnd);
			///@ todo window id 
		}


	protected:
		virtual int init();
		virtual int mainloop();
		virtual void release();

	public:
		virtual LRESULT CALLBACK MessageHandler(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

private:
	int m_screenWidth;
	int m_screenHeight;

	std::vector<UI::Window*> m_windows;
	
};
