/**
	Platform extenson to ingegrate GK SDK and Imgui
*/

#include "core/system.h"
#include "core/window.h"

#include "render/renderer.h"

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

	protected:
		virtual int init();
		virtual int mainloop();
		virtual void release();

	public:
		virtual LRESULT CALLBACK MessageHandler(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

private:
	int m_screenWidth;
	int m_screenHeight;

	
};
