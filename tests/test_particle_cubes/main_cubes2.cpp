#include "core/system.h"

#include "render/renderer.h"
#include "render/Scene.h"
#include "render/camera.h"
#include "render/model.h"
#include "render/texture.h"
#include "render/Material.h"
#include "render/shader.h"

#include "math/matrix.h"

#include "math/fbm.h"

#include "utils/asset.h"
#include "utils/AssetFactory.h"
#include "utils/AssetFile.h"

#include "utils/ResourceManager.h"

#include "generator/SceneLoader.h"

#include "render/compute.h"

#include "generator/TextureLoader.h"
#include "generator/ShaderLoader.h"

using namespace Grafkit;

#include "builtin_data/cube.h"

#define CUBEW 64
#define CUBEH 64
#define CUBEWH ((CUBEW) * (CUBEH))

class Application : public System, public IResourceManager
{
public:
	Application() : System(),
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

		this->render.Initialize(screenWidth, screenHeight, VSYNC_ENABLED, this->m_window.getHWnd(), FULL_SCREEN);

		// init file loader
		this->m_file_loader = new FileAssetFactory("./../assets/");
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

	ComputeRef liquidCompute;
	ShaderResRef fsLiquid;

	int init() {

		// --------------------------------------------------
		CameraRef camera = new Camera;
		camera->SetName("camera");

		TextureResRef texture = new TextureRes();
		texture = this->Load<TextureRes>(new TextureFromBitmap("Untitled.png", "textures/Untitled.png"));

		// -- texture sampler
		textureSampler = new TextureSampler();
		textureSampler->Initialize(render);

		// -- load shader
		vs = Load<ShaderRes>(new VertexShaderLoader("vShader", "shaders/vertexparticle.hlsl", ""));
		fs = Load<ShaderRes>(new PixelShaderLoader("pShader", "shaders/flat.hlsl", ""));

		fsLiquid = Load<ShaderRes>(new PixelShaderLoader("LiquidCompute", "shaders/liquid.hlsl", "LiquidCompute"));

		// -- precalc
		this->DoPrecalc();

		// -- model 
		ModelRef model = new Model(GrafkitData::CreateCubes(CUBEWH));
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

		// ps based compute shader mockup
		liquidCompute = new Compute();
		liquidCompute->AddChannel("tex_age");
		liquidCompute->AddChannel("tex_velocity");
		liquidCompute->AddChannel("tex_speed");
		liquidCompute->AddChannel("tex_position");

		liquidCompute->Initialize(render, fsLiquid, CUBEW);

		/* ------------------------------------------------------------ */

		this->DoPrecalc();

		scene->Get()->BuildScene(render, vs, fs);
		scene->Get()->SetActiveCamera(0);

		// --- 

		rootActor = scene->Get()->GetRootNode();
		cameraActor = scene->Get()->GetActiveCameraNode();

		this->t = 0;

		return 0;
	};

	// ==================================================================================================================

	void release() {
		this->render.Shutdown();
	};

	// ==================================================================================================================
	int mainloop() {

		liquidCompute->Render(render);

		this->render.BeginSceneDev();
		{
			liquidCompute->BindOutputs(render, (*vs));
			scene->Get()->PreRender(render);
			scene->Get()->Render(render);

			this->t += 0.1f;
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
