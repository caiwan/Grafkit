#include "core/system.h"

#include "render/renderer.h"
#include "render/camera.h"
#include "render/model.h"

using FWrender::Renderer;
using FWrender::Camera;
using FWrender::Model;

class Application : public FWcore::System
{  
public:
		Application()  : FWcore::System()
		{
			int screenWidth, screenHeight;

			screenWidth = 800;
			screenHeight = 600;

			// initwindowot is ertelmesebb helyre kell rakni
			InitializeWindows(screenWidth, screenHeight);
		}
		
		~Application() {
		}

		Renderer *render;
		Camera *render;
		Model *render;
		
		int init() {
			this->render = new Renderer();
			int 
			this->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);

			return 0;
		};
		
		void release() {
		};
		
		int mainloop() {
			return 0;
		};
	
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	Application app;
	return app.execute();
}