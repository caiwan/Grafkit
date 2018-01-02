#include "core/system.h"

#include "render/renderer.h"
#include "render/Scene.h"
#include "render/camera.h"
#include "render/model.h"
#include "render/texture.h"
#include "render/Material.h"
#include "render/shader.h"

#include "render/particle.h"

#include "math/matrix.h"

#include "math/fbm.h"

#include "utils/asset.h"
#include "utils/AssetFactory.h"
#include "utils/AssetFile.h"

#include "utils/ResourceManager.h"

#include "generator/SceneLoader.h"

#include "render/compute.h"
#include "render/effect.h"

#include "generator/TextureLoader.h"
#include "generator/ShaderLoader.h"

using namespace Grafkit;

#include "builtin_data/cube.h"

#define PARTICLE_RES 128
#define PARTICLE_COUNT ((PARTICLE_RES) * (PARTICLE_RES))

class Application : public System, public IResourceManager
{
public:
	Application() : System(),
		m_file_loader(nullptr)
	{
		int screenWidth, screenHeight;

#if 0
		screenWidth = 800;
		screenHeight = 600;
#else
		screenWidth = 1024;
		screenHeight = 768;
#endif

		t = 0;

		// initwindowot is ertelmesebb helyre kell rakni
		InitializeWindows(screenWidth, screenHeight);

		// --- ezeket kell osszeszedni egy initwindowban
		screenWidth = m_window.getRealWidth(), screenHeight = m_window.getRealHeight();
		const int VSYNC_ENABLED = 1, FULL_SCREEN = 0;

		this->render.Initialize(screenWidth, screenHeight, VSYNC_ENABLED, this->m_window.getHWnd(), FULL_SCREEN);

		// init file loader
#ifndef LIVE_RELEASE
		this->m_file_loader = new FileAssetFactory("./../assets/");
#else
		this->m_file_loader = new FileAssetFactory("./assets/");
#endif
	}

	~Application() {
	}

protected:
	Renderer render;
	SceneResRef scene;

	TextureSamplerRef textureSampler;

	ActorRef rootActor;
	ActorRef cameraActor;

	float t;

	ShaderResRef vs;
	ShaderResRef fs;
	ShaderResRef m_fxFXAA;

	Ref<ParticleEngine> particleEngine;

	EffectComposerRef m_postfx;

	ShaderResRef fsParticleEngine;

	int init() {

		// --------------------------------------------------
		CameraRef camera = new Camera;
		camera->SetName("camera");

		TextureResRef texture = new Texture2DRes();
		texture = this->Load<Texture2DRes>(new TextureFromBitmap("Untitled.png", "textures/Untitled.png"));

		// -- texture sampler
		textureSampler = new TextureSampler();
		textureSampler->Initialize(render);

		// -- load shader
		vs = Load<ShaderRes>(new VertexShaderLoader("vShader", "shaders/vertexparticle.hlsl", ""));
		fs = Load<ShaderRes>(new PixelShaderLoader("pShader", "shaders/flat.hlsl", ""));

		m_fxFXAA = Load<ShaderRes>(new PixelShaderLoader("xFXAA", "shaders/fxaa.hlsl", "FXAA"));

		fsParticleEngine = Load<ShaderRes>(new PixelShaderLoader("ParticleCompute", "shaders/particles.hlsl", "ParticleCompute"));

		// -- precalc
		this->DoPrecalc();

		// -- model 
		ModelRef model = new Model(GrafkitData::CreateCubes(PARTICLE_COUNT));
		model->SetMaterial(new Material());
		model->GetMaterial()->AddTexture(texture, Material::TT_diffuse);
		model->GetMaterial()->SetName("GridMaterial");

		model->SetName("cube");
		model->GetMesh()->Build(render, vs);

		// -- setup scene 
		scene = new SceneRes(new Scene());

		cameraActor = new Actor();
		cameraActor->SetName("cameraNode");
		cameraActor->AddEntity(camera);

		/* Kocka kozepen */
		ActorRef modelActor = new Actor(model);
		modelActor->SetName("center");

		/*
		Add cubes to the root
		*/
		rootActor = new Actor();
		rootActor->SetName("root");
		rootActor->AddChild(cameraActor);
		rootActor->AddChild(modelActor);

		scene->Get()->Initialize(rootActor);

		scene->Get()->SetActiveCamera(0);

		// add shaders
		scene->Get()->SetVShader(vs);
		scene->Get()->SetPShader(fs);

		cameraActor->Matrix().Identity();
		cameraActor->Matrix().LookAtLH(float3(100, 100, 100));

		particleEngine = new ParticleEngine();
		particleEngine->AddDynamics(new ParticleAttractor(float4(0, 400, 0, 1), 1., 5.25));
		particleEngine->AddDynamics(new ParticleAttractor(float4(0, -400, 0, 1), 1., 5.25));
		particleEngine->AddDynamics(new ParticleAttractor(float4(400, 0, 0, 1), 1., -5.25));
		particleEngine->AddDynamics(new ParticleAttractor(float4(-400, 0, 0, 1), 1., -5.25));


		auto particleGlobals = particleEngine->GetGlobals();
		particleGlobals.maxAge = 10;
		particleGlobals.emitRate = 5;
		particleGlobals.brownianScale = .001;
		particleGlobals.brownianSpeed = 8.;
		particleGlobals.speedScale = 1.;

		particleEngine->SetGlobals(
			particleGlobals
		);

		particleEngine->Initialize(render, fsParticleEngine, PARTICLE_RES);

		/* ------------------------------------------------------------ */

		scene->Get()->BuildScene(render, vs, fs);
		scene->Get()->SetActiveCamera(0);

		// --- 

		rootActor = scene->Get()->GetRootNode();
		cameraActor = scene->Get()->GetActiveCameraNode();

		//
		m_postfx = new EffectComposer();
		m_postfx->AddPass(new EffectPass(m_fxFXAA));
		m_postfx->Initialize(render);


		ZeroMemory(&sceneParams, sizeof(sceneParams));

		this->t = 0;

		return 0;
	};

	struct
	{
		float globalTime;
		float lastTime;
		float deltaTime;
		float _padding;
		int frameCount;
	} sceneParams;

	// ==================================================================================================================

	void release() {
		this->render.Shutdown();
	};

	// ==================================================================================================================
	int mainloop() {
#ifndef LIVE_RELEASE
		updateTroubleshoot();
#endif //LIVE_RELEASE

		(*fsParticleEngine)->SetParamT(render, "SceneParams", sceneParams);

		particleEngine->Render(render);

		m_postfx->BindInput(render);

		this->render.BeginSceneDev();
		{
			particleEngine->GetCompute()->BindOutputs(render, (*vs));
			scene->Get()->RenderFrame(render, t);
		}

		m_postfx->Render(render);

		this->render.EndScene();

		sceneParams.deltaTime = render.GetDeltaTime();
		sceneParams.globalTime = t;
		sceneParams.frameCount++;
		this->t += sceneParams.deltaTime;

		// livereload
#ifndef  LIVE_RELEASE
		if (m_file_loader->PollEvents(this)) {
			sceneParams.frameCount = 0;
		}
#endif // ! LIVE_RELEASE

		int res = 0;
		res |= isEscPressed();

		return res;
	};


#ifndef LIVE_RELEASE
	struct troubleshootPar_t {
		troubleshootPar_t() { showMap = 0, showOutput = 0; }
		union {
			struct {
				int showOutput;
				int showMap;
			};
			float4 _;
		};
	};

	troubleshootPar_t troubleshootPar;

	void updateTroubleshoot() {

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

		if (m_pInput->IsKeyDown(VK_SPACE))
			sceneParams.frameCount = 0;
	}

#endif //LIVE_RELEASE

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
