#define _USE_MATH_DEFINES
#include <cmath>

#include "core/system.h"

#include "render/renderer.h"
#include "render/camera.h"
#include "render/Material.h"
#include "render/shader.h"

#include "render/effect.h"

#include "generator/ShaderLoader.h"
#include "generator/TextureLoader.h"

#include "utils/InitializeSerializer.h"

#include "generator/SceneLoader.h"
#include "render/Scene.h"

#include "math/matrix.h"

#include "utils/AssetFile.h"
#include "utils/ResourceManager.h"
#include "utils/ResourcePreloader.h"

using namespace Grafkit;

class Application : public Grafkit::System, protected Grafkit::ResourcePreloader, protected Grafkit::ClonableInitializer
{
public:
	Application() : Grafkit::System(),
		m_file_loader(nullptr)
	{
		int screenWidth, screenHeight;

		screenWidth = 800;
		screenHeight = 600;

		// initwindowot is ertelmesebb helyre kell rakni
		InitializeWindows(screenWidth, screenHeight);

		//this->render = new Renderer();

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

	ActorRef currCameraActor;
	CameraRef camera;

	TextureSamplerRef sampler;

	TextureCubeResRef envmap;

	EffectComposerRef drawCubemap;

	SceneResRef scene;

	float m_t;

	ShaderResRef m_vs;
	ShaderResRef m_fs;

	ShaderResRef cubemapShader;

	int init() {
		LoadCache();

		// -- load shader
		m_vs = Load<ShaderRes>(new VertexShaderLoader("vShader", "shaders/vertex.hlsl", ""));
		m_fs = Load<ShaderRes>(new PixelShaderLoader("pShader", "shaders/flat.hlsl", ""));

		// -- model 
		//scene = this->Load<SceneRes>(new SceneLoader("scene", "box.scene"));
		//scene = this->Load<SceneRes>(new SceneLoader("scene", "locRotScale.scene"));
		//scene = this->Load<SceneRes>(new SceneLoader("scene", "locRotCamera.scene"));
		scene = this->Load<SceneRes>(new SceneLoader("scene", "animtest2.scene"));

		cubemapShader = Load<ShaderRes>(new PixelShaderLoader("cubemapShader", "shaders/cubemap.hlsl", ""));

		envmap = Load<TextureCubeRes>(new TextureCubemapFromBitmap("envmap",
			"textures/yoko_posx.jpg",
			"textures/yoko_negx.jpg",
			"textures/yoko_posy.jpg",
			"textures/yoko_negy.jpg",
			"textures/yoko_posz.jpg",
			"textures/yoko_negz.jpg"
		));


		DoPrecalc();

		drawCubemap = new EffectComposer();
		drawCubemap->AddPass(new EffectPass(cubemapShader));
		drawCubemap->Initialize(render, true);

		scene->Get()->BuildScene(render, m_vs, m_fs);
		//m_scene->Get()->SetActiveCamera(0);
		scene->Get()->SetActiveCamera("Camera");
		//scene->Get()->SetActiveCamera("MainCamera");

		currCameraActor = scene->Get()->GetActiveCamera();
		camera = dynamic_cast<Camera*>(currCameraActor->GetEntities()[0].Get());
		if (camera.Valid()) {
			//c->SetUp(0, 0, 1);
			//c->SetLookTo(0, -1, 0);

			//c->SetLookAt(0, 0, 0);
			//c->SetPosition(10, -10, 10);
			//c->SetLookTo(0, 0, 0);
		}


		sampler = new TextureSampler();
		sampler->Initialize(render);

		m_t = 0;

		return 0;
	};

	void release() {
		SaveCache();
		RemoveAll();
	};

	int mainloop() 
	{

		struct {
			float2 res;
			float ar;
			float fov;
		} resprops;

		Scene::WorldMatrices_t worldMatrices;

		this->render.BeginScene();
		{

			this->scene->Get()->UpdateAnimation(fmod(m_t, 10.5));
			this->scene->Get()->PreRender(render);

			resprops.ar = render.GetAspectRatio();
			resprops.fov = camera->GetFOV();

			worldMatrices = (*scene)->GetWorldMatrices();

			(*cubemapShader)->SetParam(render, "ResolutionBuffer", &resprops);
			(*cubemapShader)->SetParam(render, "MatrixBuffer", &worldMatrices);
			(*cubemapShader)->SetShaderResourceView("skybox", (*envmap)->GetShaderResourceView());
			(*cubemapShader)->SetSamplerSatate("SampleType", sampler->GetSamplerState());

			render.ToggleDepthWrite(false);

			drawCubemap->Render(render);

			render.ToggleDepthWrite(true);

			m_t += .01;
			
			(*m_fs)->SetSamplerSatate("SampleType", sampler->GetSamplerState());
			this->scene->Get()->Render(render);

		}

		this->render.EndScene();

		//this->GetAssetFactory()->PollEvents(this);

		return 0;
	};

private:
	IAssetFactory *m_file_loader;
public:
	IAssetFactory *GetAssetFactory() { return m_file_loader; }
	Renderer & GetDeviceContext() { return render; }
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	Application app;
	return app.execute();
}
