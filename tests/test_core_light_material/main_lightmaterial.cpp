#define _USE_MATH_DEFINES
#include <cmath>

#include "core/system.h"

#include "render/renderer.h"
#include "render/camera.h"
#include "render/Material.h"
#include "render/shader.h"
#include "render/light.h"

#include "generator/ShaderLoader.h"
#include "generator/SceneLoader.h"

#include "render/Scene.h"

#include "math/matrix.h"

#include "utils/AssetFile.h"
#include "utils/ResourceManager.h"
#include "utils/ResourcePreloader.h"

#include "utils/InitializeSerializer.h"



using namespace Grafkit;

#include "builtin_data/cube.h"

class Application : public Grafkit::System, protected Grafkit::ResourcePreloader, private Grafkit::ClonableInitializer
{
public:
	Application() : ClonableInitializer(), Grafkit::System(), ResourcePreloader(),
		m_file_loader(nullptr)
	{
		int screenWidth, screenHeight;

		screenWidth = 800;
		screenHeight = 600;

		t = 0;

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
	SceneResRef scene;

	TextureSamplerRef texSampler;

	LightRef light;
	ActorRef lightActor;

	ActorRef m_rootActor;
	ActorRef m_cameraActor;

	ActorRef camera;

	float t;

	ShaderResRef vs;
	ShaderResRef fs;

	int init() {
		LoadCache();

		// -- load shader
		vs = Load<ShaderRes>(new VertexShaderLoader("vShader", "shaders/lightmaterial.hlsl", ""));
		fs = Load<ShaderRes>(new PixelShaderLoader("pShader", "shaders/lightmaterial.hlsl", "mainPixel_DiffuseColor"));

		// -- model 
		scene = this->Load<SceneRes>(new SceneLoader("scene", "spheres.scene"));

		DoPrecalc();

		scene->Get()->BuildScene(render, vs, fs);
		scene->Get()->SetActiveCamera(0);

		camera = scene->Get()->GetActiveCamera();

		// -- add lights
		light = new PointLight();
		lightActor = new Actor(light);

		(*scene)->GetRootNode()->AddChild(lightActor);

		// --- serialize && deserialize

		this->t = 0;

		return 0;
	};

	// ==================================================================================================================

	void release() {
		this->render.Shutdown();
	};

	// ==================================================================================================================
	int mainloop() {
		this->render.BeginScene();
		{
			t += .01;

			//camera->Matrix().Identity();
			//camera->Matrix().Translate(0, 0, -10);
			//camera->Matrix().RotateRPY(t, .23847*t, .392847*t);

			this->scene->Get()->UpdateAnimation(fmod(t, 10.5));
			this->scene->Get()->PreRender(render);
			this->scene->Get()->Render(render);

		}

		this->render.EndScene();

		return 0;
	};

private:
	FileAssetFactory *m_file_loader;

public:
	IAssetFactory* GetAssetFactory() { return m_file_loader; };
	Renderer & GetDeviceContext() { return this->render; };
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	Application app;
	return app.execute();
}
