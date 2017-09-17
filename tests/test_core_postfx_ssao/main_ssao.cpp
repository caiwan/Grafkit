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
#include "generator/TextureLoader.h"

#include "render/Scene.h"
#include "render/Model.h"

#include "render/effect.h"
#include "render/camera.h"

#include "math/matrix.h"
#include "math/halton.h"

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

	TextureResRef texture;

	TextureSamplerRef sampler;
	TextureRef normalMap;
	TextureRef positionMap;
	TextureResRef noiseMap;

	EffectComposerRef postfx;

	TextureSamplerRef texSampler;

	LightRef light;
	ActorRef lightActor;


	ActorRef cameraActor;
	CameraRef camera;

	float t;

	float4 *kernels;

	ShaderResRef vs;
	ShaderResRef fs, aofs, blurfs;
	ShaderResRef cubemapShader;


	int init() {

		texture = this->Load<TextureRes>(new TextureFromBitmap("Untitled.png", "textures/Untitled.png"));

		// -- load shader
		vs = Load<ShaderRes>(new VertexShaderLoader("vShader", "shaders/vertex.hlsl", ""));
		fs = Load<ShaderRes>(new PixelShaderLoader("pShader", "shaders/flat.hlsl", ""));

		aofs = Load<ShaderRes>(new PixelShaderLoader("fxSSAOShader", "shaders/ssao.hlsl", "SSAO"));

		blurfs = Load<ShaderRes>(new PixelShaderLoader("fxblur", "shaders/blur.hlsl", "blur3x3"));

		// -- model 
#if 0
		scene = this->Load<SceneRes>(new SceneLoader("scene", "ao.scene"));
#else
		camera = new Camera;
		camera->SetName("camera");

		ModelRef model = new Model(GrafkitData::CreateCube());
		model->SetMaterial(new Material());
		model->GetMaterial()->AddTexture(texture, Material::TT_diffuse);
		model->GetMaterial()->SetName("GridMaterial");

		// -- setup scene 
		scene = new SceneRes(new Scene());

		cameraActor = new Actor();
		cameraActor->SetName("cameraNode");
		cameraActor->AddEntity(camera);

		cameraActor->Matrix().Translate(10, 10, -10);
		cameraActor->Matrix().RotateRPY(M_PI / 4, M_PI / 4, M_PI / 4);

		ActorRef modelActor = new Actor();
		modelActor->SetName("center");

#define N 9
		for (int x = 0; x < N; x++) {
			for (int y = 0; y < N; y++) {
				ActorRef actor = new Actor();
				actor->AddEntity(model);
				actor->Matrix().Translate(x - N / 2, .5 * ((float)(rand() % 256)) / 256., y - N / 2);
				modelActor->AddChild(actor);
			}
		}
#undef N

		ActorRef rootActor;
		rootActor = new Actor();
		rootActor->SetName("root");
		rootActor->AddChild(cameraActor);
		rootActor->AddChild(modelActor);

		(*scene)->Initialize(rootActor);

#endif
		// -- generate some random texture

		noiseMap = Load<TextureRes>(new TextureNoiseMap(256));

		kernels = new float4[128];
		Halton3D::HemiSphereDistribution(kernels, 128);

		DoPrecalc();

		sampler = new TextureSampler();
		sampler->Initialize(render);

		normalMap = new Texture2D();
		normalMap->InitializeFloatRGBA(render);

		positionMap = new Texture2D();
		positionMap->InitializeFloatRGBA(render);

		postfx = new EffectComposer();
		postfx->AddPass(new EffectPass(aofs));
		postfx->AddPass(new EffectPass(blurfs));
		postfx->SetInput(1, normalMap);
		postfx->SetInput(2, positionMap);
		postfx->Initialize(render);

		(*aofs)->SetSamplerSatate(render, "SampleType", sampler->GetSamplerState());
		(*aofs)->SetShaderResourceView(render, "normalMapTexture", normalMap->GetShaderResourceView());
		(*aofs)->SetShaderResourceView(render, "viewMapTexture", positionMap->GetShaderResourceView());
		(*aofs)->SetShaderResourceView(render, "noiseMap", (*noiseMap)->GetShaderResourceView());
		(*aofs)->SetParam(render, "ssaoKernelBuffer", kernels);

		(*blurfs)->SetSamplerSatate(render, "SampleType", sampler->GetSamplerState());

		// ...

		(*scene)->BuildScene(render, vs, fs);
		(*scene)->SetActiveCamera(0);

		cameraActor = (*scene)->GetActiveCameraNode();
		camera = (*scene)->GetActiveCamera();

		(*fs)->SetSamplerSatate(render, "SampleType", sampler->GetSamplerState());

		this->t = 0;

		return 0;
	};

	// ==================================================================================================================

	void release() {
		this->render.Shutdown();

		if (kernels)
			delete[] kernels;
		kernels = nullptr;
	};

	// ==================================================================================================================
	int mainloop() {
		float4 ssaoparams = float4(10, 10, 1., .25);

		Scene::WorldMatrices_t worldMatrices;

		postfx->BindInput(render);

		this->render.BeginScene();
		{
			t += .01;

			this->scene->Get()->UpdateAnimation(fmod(t, 10.5));
			this->scene->Get()->PreRender(render);

			worldMatrices = (*scene)->GetWorldMatrices();
			this->scene->Get()->Render(render);
		}

		(*aofs)->SetParam(render, "ssaoParamBuffer", &ssaoparams);
		(*aofs)->SetParam(render, "MatrixBuffer", &worldMatrices);

		postfx->Render(render);

		this->render.EndScene();

		return isEscPressed();
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
