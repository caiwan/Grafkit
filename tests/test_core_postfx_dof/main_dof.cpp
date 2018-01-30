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

#define BOKEH_APERTURE 0.37433372

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
	TextureRef colorMap;
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
	ShaderResRef fs;
	ShaderResRef aoFs, blurFs;

	// dof 
	struct {

		TextureRef blurCalcMap;
		TextureRef blurInput[3];

		struct {
			float Aperture;
			float Focus;
			float angle;
		} blurParam[6];

		struct {
			//https://en.wikipedia.org/wiki/Circle_of_confusion
			float Aperture;
			float Focus; // multiplied with distanceUnit (see above)
			float Limit; // blur limit, screen space
		} cocParam;

		EffectComposerRef fxPrecalc;
		EffectComposerRef fxPass[3];
		EffectComposerRef fxCombine;

		ShaderResRef fsPrecalc;
		ShaderResRef fsBlur;
		ShaderResRef fsCombine;

	} dof;

#ifndef LIVE_RELEASE
	ShaderResRef troubleshootFs;
#endif // !LIVE_RELEASE


	int init() {

		texture = this->Load<Texture2DRes>(new TextureFromBitmap("Untitled.png", "textures/Untitled.png"));
		noiseMap = Load<Texture2DRes>(new TextureNoiseMap(256));

		// -- load shader
		vs = Load<ShaderRes>(new VertexShaderLoader("vShader", "shaders/vertex.hlsl", ""));
		fs = Load<ShaderRes>(new PixelShaderLoader("pShader", "shaders/flat.hlsl", ""));

		aoFs = Load<ShaderRes>(new PixelShaderLoader("fxSSAOShader", "shaders/ssao.hlsl", "SSAO"));
		blurFs = Load<ShaderRes>(new PixelShaderLoader("fxSSAOSmooth", "shaders/ssao.hlsl", "SSAOSmooth"));

		// dof
		{
			dof.fsPrecalc = Load<ShaderRes>(new PixelShaderLoader("fxDofCalc", "shaders/dof.hlsl", "calculateBlurAmount"));
			dof.fsBlur = Load<ShaderRes>(new PixelShaderLoader("fxDofBlur", "shaders/dof.hlsl", "blur"));
			dof.fsCombine = Load<ShaderRes>(new PixelShaderLoader("fxDofcombine", "shaders/dof.hlsl", "combine"));
		}

#ifndef LIVE_RELEASE
		troubleshootFs = Load<ShaderRes>(new PixelShaderLoader("fxDebug", "shaders/troubleshoot.hlsl", "debugFx"));
#endif // !LIVE_RELEASE

		// -- model 
#if 0
		scene = this->Load<SceneRes>(new SceneLoader("scene", "ao.scene"));
#else
		// 
		DoPrecalc();
		//

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

		cameraActor->Matrix().LookAtLH(float3(10, 10, -10));

		ActorRef modelActor = new Actor();
		modelActor->SetName("center");

#define N 9
		for (int x = 0; x < N; x++) {
			for (int y = 0; y < N; y++) {
				ActorRef actor = new Actor();
				actor->AddEntity(model);

				float xx = x - N / 2;
				float yy = y - N / 2;
				float zz = (float)(rand() % 256) / 256.;

				actor->Matrix().Scale(.5, .5, .5);
				actor->Matrix().Translate(xx, zz, yy);
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

		kernels = new float4[128];
		Halton3D::HemiSphereDistribution(kernels, 128);


		sampler = new TextureSampler();
		sampler->Initialize(render);

		colorMap = new Texture2D();
		colorMap->Initialize(render);

		normalMap = new Texture2D();
		normalMap->InitializeFloatRGBA(render);

		positionMap = new Texture2D();
		positionMap->InitializeFloatRGBA(render);

		postfx = new EffectComposer();
		postfx->AddPass(new EffectPass(aoFs));
		postfx->AddPass(new EffectPass(blurFs));

#ifndef LIVE_RELEASE
		//postfx->AddPass(new EffectPass(troubleshootFs));
#endif

		postfx->SetInput(1, normalMap);
		postfx->SetInput(2, positionMap);

		postfx->SetOutput(colorMap);

		postfx->Initialize(render);

		// Depth of field
		{
			dof.fxPrecalc = new EffectComposer();
			dof.fxPrecalc->AddPass(new EffectPass(dof.fsPrecalc));

			//ZeroMemory(&dof.cocParam, sizeof(dof.cocParam));

			dof.cocParam.Aperture = 0;
			dof.cocParam.Focus = 0;
			dof.cocParam.Limit = 0;

			dof.blurCalcMap = new Texture2D();
			dof.blurCalcMap->Initialize(render);

			dof.fxPrecalc->SetOutput(dof.blurCalcMap);

			//(*dof.fsBlur)->SetParamT(render, "CircleOfConfusionParams", dof.cocParam);
			dof.fxPrecalc->GetPass(0)->GetParameter()->SetParam("CircleOfConfusionParams", &dof.cocParam);

			for (int i = 0; i < 3; i++) {
				dof.fxPass[i] = new EffectComposer();
				dof.fxPass[i]->AddPass(new EffectPass(dof.fsBlur));
				dof.fxPass[i]->AddPass(new EffectPass(dof.fsBlur));
				dof.fxPass[i]->Initialize(render);

				dof.fxPass[i]->GetPass(0)->GetParameter()->SetParam("BlurParams", &dof.blurParam[2 * i + 0]);
				dof.fxPass[i]->GetPass(1)->GetParameter()->SetParam("BlurParams", &dof.blurParam[2 * i + 1]);

				dof.blurInput[i] = new Texture2D();
				dof.blurInput[i]->Initialize(render);

				dof.fxPass[i]->SetOutput(dof.blurInput[i]);
			}

			const float angles[] = {
				1., 3., 3., 5., 5., 1.
			};

			for (int i = 0; i < 6; i++) {
				//ZeroMemory(&dof.blurParam[i], sizeof(dof.blurParam[i]));

				dof.blurParam[i].angle = angles[i];
				dof.blurParam[i].Aperture = 0;
				dof.blurParam[i].Focus = 0;
			}

			dof.fxCombine = new EffectComposer();
			dof.fxCombine->AddPass(new EffectPass(dof.fsCombine));
			dof.fxCombine->Initialize(render);

		}
		// 

		(*scene)->BuildScene(render, vs, fs);
		(*scene)->SetActiveCamera(0);

		cameraActor = (*scene)->GetActiveCameraNode();
		camera = (*scene)->GetActiveCamera();

		setupMaps();

		this->t = 0;

		troubleshootPar.showMap = 0;
		troubleshootPar.showOutput = 0;

		// 
		float w, h;
		render.GetScreenSizef(w, h);
		ssaopar.noiseScele.x = w / 256;
		ssaopar.noiseScele.y = h / 256;

		ssaopar.radius = .5;
		ssaopar.treshold = .5;

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

	void setupMaps() {
#ifndef LIVE_RELEASE
		(*troubleshootFs)->SetSamplerSatate(render, "SampleType", sampler->GetSamplerState());
		(*troubleshootFs)->SetShaderResourceView(render, "normalMap", normalMap->GetShaderResourceView());
		(*troubleshootFs)->SetShaderResourceView(render, "viewMap", positionMap->GetShaderResourceView());
#endif //LIVE_RELEASE

		(*aoFs)->SetSamplerSatate(render, "SampleType", sampler->GetSamplerState());
		(*aoFs)->SetShaderResourceView(render, "normalMap", normalMap->GetShaderResourceView());
		(*aoFs)->SetShaderResourceView(render, "viewMap", positionMap->GetShaderResourceView());
		(*aoFs)->SetShaderResourceView(render, "noiseMap", (*noiseMap)->GetShaderResourceView());
		(*aoFs)->SetParam(render, "ssaoKernelBuffer", kernels);

		(*blurFs)->SetSamplerSatate(render, "SampleType", sampler->GetSamplerState());

		(*fs)->SetSamplerSatate(render, "SampleType", sampler->GetSamplerState());
	}

	struct {
		float2 noiseScele;
		float radius;
		float treshold;
	}ssaopar;

#ifndef LIVE_RELEASE
	struct {
		int showOutput;
		int showMap;
		char _[2 * 4];
	} troubleshootPar;
#endif LIVE_RELEASE

	// ==================================================================================================================

	int mainloop() {
#ifndef LIVE_RELEASE
		setupMaps();

		int key = -1;
		for (int i = 0; i < 8; i++) {
			if (m_pInput->IsKeyDown(VK_F1 + i)) {
				key = i; break;
			}
		}
		if (key != -1) {
			if (m_pInput->IsKeyDown(VK_SHIFT)) { troubleshootPar.showOutput = key; }
			else { troubleshootPar.showMap = key; }
		}

		(*troubleshootFs)->SetParam(render, "TroubleshootParams", &troubleshootPar);
#endif //LIVE_RELEASE

		(*aoFs)->SetParam(render, "ssaoParamBuffer", &ssaopar);

		// 
		Scene::WorldMatrices_t worldMatrices;


		// set parameters
		{
			dof.cocParam.Aperture = BOKEH_APERTURE / 2.;
			dof.cocParam.Focus = 1. + .5* sin(.5*t);
			dof.cocParam.Limit = .125;

			for (int i = 0; i < 6; i++) {
				dof.blurParam[i].Aperture = dof.cocParam.Aperture;
				dof.blurParam[i].Focus = dof.cocParam.Focus;
			}
		}

		postfx->BindInput(render);

		this->render.BeginScene();
		{
			t += .01;

			this->scene->Get()->UpdateAnimation(fmod(t, 10.5));
			this->scene->Get()->PreRender(render);

			worldMatrices = (*scene)->GetWorldMatrices();
			this->scene->Get()->Render(render);
		}

		//
		(*aoFs)->SetParam(render, "MatrixBuffer", &worldMatrices);

		postfx->Render(render);

		{
			(*dof.fsPrecalc)->SetShaderResourceView(render, "color", *colorMap);
			(*dof.fsPrecalc)->SetShaderResourceView(render, "depth", *positionMap);
			dof.fxPrecalc->Render(render);

			(*dof.fsBlur)->SetShaderResourceView(render, "input", *dof.blurCalcMap);

			for (int i = 0; i < 3; i++) {
				dof.fxPass[i]->SetInput(0, dof.blurCalcMap);
				dof.fxPass[i]->Render(render);
			}

			(*dof.fsCombine)->SetShaderResourceView(render, "input1", *dof.blurInput[0]);
			(*dof.fsCombine)->SetShaderResourceView(render, "input2", *dof.blurInput[1]);
			(*dof.fsCombine)->SetShaderResourceView(render, "input3", *dof.blurInput[2]);

			dof.fxCombine->Render(render);
		}

		this->render.EndScene();

		m_file_loader->PollEvents(this);

		return isEscPressed();
	};

	// ==================================================================================================================

private:
	FileAssetFactory *m_file_loader;

public:
	IAssetFactory* GetAssetFactory() { return m_file_loader; };
	Renderer & GetDeviceContext() { return this->render; };
};

// ==================================================================================================================

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	Application app;
	return app.execute();
}
