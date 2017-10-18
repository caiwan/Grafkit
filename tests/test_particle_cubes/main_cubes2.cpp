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

	TextureSamplerRef m_textureSampler;

	ActorRef m_rootActor;
	ActorRef m_cameraActor;

	float t;

	ShaderResRef m_vertexShader;
	ShaderResRef m_fragmentShader;

	int init() {

		// --------------------------------------------------
		CameraRef camera = new Camera;
		camera->SetName("camera");

		TextureResRef texture = new TextureRes();
		texture = this->Load<TextureRes>(new TextureFromBitmap("Untitled.png", "textures/Untitled.png"));

		// -- texture sampler
		m_textureSampler = new TextureSampler();
		m_textureSampler->Initialize(render);

		// -- load shader
		m_vertexShader = Load<ShaderRes>(new VertexShaderLoader("vShader", "shaders/vertexgroups.hlsl", ""));
		m_fragmentShader = Load<ShaderRes>(new PixelShaderLoader("pShader", "shaders/flat.hlsl", ""));

		// -- precalc
		this->DoPrecalc();

		// -- model 
		ModelRef model = new Model(GrafkitData::CreateCubes(CUBEWH));
		model->SetMaterial(new Material());
		model->GetMaterial()->AddTexture(texture, Material::TT_diffuse);
		model->GetMaterial()->SetName("GridMaterial");

		model->SetName("cube");
		model->GetMesh()->Build(render, m_vertexShader);

		// -- setup scene 
		scene = new SceneRes(new Scene());

		m_cameraActor = new Actor();
		m_cameraActor->SetName("cameraNode");
		m_cameraActor->AddEntity(camera);

		/* Kocka kozepen */
		ActorRef modelActor = new Actor(model);
		modelActor->SetName("center");

		/*
		Add cubes to the root
		*/
		m_rootActor = new Actor();
		m_rootActor->SetName("root");
		m_rootActor->AddChild(m_cameraActor);
		m_rootActor->AddChild(modelActor);

		scene->Get()->Initialize(m_rootActor);

		scene->Get()->SetActiveCamera(0);

		// add shaders
		scene->Get()->SetVShader(m_vertexShader);
		scene->Get()->SetPShader(m_fragmentShader);

		m_cameraActor->Matrix().Identity();
		m_cameraActor->Matrix().LookAtLH(float3(100, 100, 100));

		/* ------------------------------------------------------------ */

		this->DoPrecalc();

		scene->Get()->BuildScene(render, m_vertexShader, m_fragmentShader);
		scene->Get()->SetActiveCamera(0);

		// --- 

		m_rootActor = scene->Get()->GetRootNode();
		m_cameraActor = scene->Get()->GetActiveCameraNode();

		this->t = 0;

		return 0;
	};

	// ==================================================================================================================

	void release() {
		this->render.Shutdown();
	};

	// ==================================================================================================================

	struct cube_params_t{
		cube_params_t(){}
		union {
			struct {
				int cubew, cubeh;
			}; float4 _p;
		};
		float4 p[CUBEW];
	};

	struct cube_params_t cube_params;

	int mainloop() {
		cube_params.cubew = CUBEW;
		cube_params.cubeh = CUBEH;

		for (int i = 0; i < 64; i++) {
			cube_params.p[i] = float4(0,0,0,0);
			cube_params.p[i].x = fbm(float2(0, (float)i / 64.));
			cube_params.p[i].y = fbm(float2((float)i / 64., 0.));
		}

		this->render.BeginSceneDev();
		{
			(*m_vertexShader)->SetParamT(render, "cube_params", cube_params);
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
