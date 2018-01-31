#include "demo/demoframework.h"

#include "utils/AssetFile.h"

#include "cubeset.h"

using namespace Grafkit;

#define WINDOW_W 800
#define WINDOW_H 600

class Application : public GKDemo::DemoApplication {

public:
	Application() : GKDemo::DemoApplication()
	{
		int screenWidth = WINDOW_W, screenHeight = WINDOW_H;
		const int VSYNC_ENABLED = 1, FULL_SCREEN = 0;

		InitializeWindows(screenWidth, screenHeight);
		screenWidth = m_window.getRealWidth(), screenHeight = m_window.getRealHeight();

		m_render.Initialize(screenWidth, screenHeight, VSYNC_ENABLED, this->m_window.getHWnd(), FULL_SCREEN);
		m_loader = new FileAssetFactory("./../assets/");

		// construct scenes
		m_cubes = new CubeScene(); m_scenes.push_back(m_cubes);
	}

private:
	Ref<CubeScene> m_cubes;

protected:
	// Define your elements here
	virtual void OnBeforePreload() {}

	// Initialize your elements after loadig their resources from disk
	virtual void OnAfterPreload() {}

	// Add your automatization tracks here if you want
	virtual void OnDelegateTracks() {}

	// This will be replaced with a more advanced (mean: less shittycoded) techology (bullshit)
	virtual void OnSetupResources() {}

	// This is where the fun part comes
	// Before render anything and/or binding any outputs
	virtual void OnBeforeRender() {}

	// Render 3D 
	virtual void OnRender() {}

	// Do the tick
	virtual void OnAfterRender() {}

};

// ==================================================================================================================

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	Application app;
	return app.execute();
}
