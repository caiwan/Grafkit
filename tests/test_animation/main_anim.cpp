#define _USE_MATH_DEFINES
#include <cmath>

#include "core/system.h"

#include "render/renderer.h"
#include "render/shader.h"

#include "testscene.h"

#include "generator/ShaderLoader.h"
//#include "generator/SceneLoader.h"

#include "animation/scene.h"

#include "math/matrix.h"

#include "utils/AssetFile.h"
#include "utils/ResourceManager.h"
#include "utils/ResourcePreloader.h"

#include "utils/InitializeSerializer.h"

using namespace Grafkit;

class Application : public Grafkit::System, protected Grafkit::ResourcePreloader, private Grafkit::ClonableInitializer
{
public:
	Application() : ClonableInitializer(), Grafkit::System(), ResourcePreloader(),
		m_file_loader(nullptr)
	{
		int screenWidth, screenHeight;

		screenWidth = 800;
		screenHeight = 600;

		// initwindowot is ertelmesebb helyre kell rakni
		InitializeWindows(screenWidth, screenHeight);

		// --- ezeket kell osszeszedni egy initwindowban
		screenWidth = m_window.getRealWidth(), screenHeight = m_window.getRealHeight();
		const int VSYNC_ENABLED = 1, FULL_SCREEN = 0;

		int result = 0;

		result = this->render.Initialize(screenWidth, screenHeight, VSYNC_ENABLED, this->m_window.getHWnd(), FULL_SCREEN);
		this->m_file_loader = new FileAssetFactory("./../assets/");
	}

	~Application() {
		delete m_file_loader;	///TODO: shutdown elotte
		this->render.Shutdown();
	}

protected:
	Renderer render;

	ActorRef m_currCameraActor;
	CameraRef m_camera;

	SceneResRef m_scene;

	float m_t;

	ShaderResRef m_vs;
	ShaderResRef m_fs;

	int init() {
		//LoadCache();

		// -- load shader
		m_vs = Load<ShaderRes>(new VertexShaderLoader("vShader", "shaders/vertex.hlsl", ""));
		m_fs = Load<ShaderRes>(new PixelShaderLoader("pShader", "shaders/flat.hlsl", ""));

		// -- model 
		DoPrecalc();

		m_scene = new SceneRes(new Scene());
		TestScene::createTestScene(m_scene);

		(*m_scene)->Initialize();
		(*m_scene)->Build(render, m_vs, m_fs);

		// add animation
		//(*m_scene)->

		const float maxTime = 2.;
		const float keyStep = 1./10.;
		for (float t = 0; t < maxTime; t += keyStep) {
			// ... 
			// let say it works for now
		}

		m_t = 0;

		return 0;
	};

	void release() {
		SaveCache();
		RemoveAll();
	};

	int mainloop() {
		this->render.BeginScene();
		{
			(*m_scene)->UpdateScene(render, m_t);
			(*m_scene)->Render(render);
			m_t += .01;

			if (m_t > 2.)
				m_t = 0;
		}

		this->render.EndScene();

		//this->GetAssetFactory()->PollEvents(this);

		//return m_t > 1.;
		return 0;
	};

private:
	IAssetFactory * m_file_loader;
public:
	IAssetFactory * GetAssetFactory() { return m_file_loader; }
	Renderer & GetDeviceContext() { return render; }
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	_CrtSetDbgFlag(_CRTDBG_CHECK_ALWAYS_DF);

	Application app;
	return app.execute();
}
