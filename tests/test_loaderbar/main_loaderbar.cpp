#include "core/system.h"

#include "render/renderer.h"

#include "utils/ResourcePreloader.h"
#include "utils/InitializeSerializer.h"

#include "utils/AssetFile.h"

#include "builtin_data/loaderbar.h"


using namespace Grafkit;
using namespace GrafkitData;

class Application : public System, protected ResourcePreloader, protected ClonableInitializer, protected LoaderBar
{
public:
	Application() : System(), ClonableInitializer(), ResourcePreloader(this), LoaderBar(),
		m_file_loader(nullptr)
	{
		int screenWidth, screenHeight;

		screenWidth = 800;
		screenHeight = 600;

		t = 0;

		// initwindowot is ertelmesebb helyre kell rakni
		InitializeWindows(screenWidth, screenHeight);

		screenWidth = m_window.getRealWidth(), screenHeight = m_window.getRealHeight();
		const int VSYNC_ENABLED = 1, FULL_SCREEN = 0;

		this->render.Initialize(screenWidth, screenHeight, VSYNC_ENABLED, this->m_window.getHWnd(), FULL_SCREEN);

		this->m_file_loader = new FileAssetFactory("./../assets/");
	}

	~Application() {
	}

protected:
	Renderer render;

	float t;

	int init() 
	{
		InitializeLoaderBar(render);

		LoadCache();
		DoPrecalc();

		t = 0;

		return 0;
	};

	// ==================================================================================================================

	void release() {
		this->render.Shutdown();
	};

	// ==================================================================================================================
	int mainloop() {

		UpdateLoaderBar(t);

		t += .01;
		if (t >= 1)
			t = 0;

		return 0;
	};

protected:
	void UpdateLoaderBar(float p) {
		DrawLoaderBar(render, p);
	}

private:
	FileAssetFactory *m_file_loader;

public:
	IAssetFactory *GetAssetFactory() { return m_file_loader; }
	Renderer & GetDeviceContext() { return this->render; }
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	Application app;
	return app.execute();
}
