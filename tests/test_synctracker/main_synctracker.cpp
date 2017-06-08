#include "core/system.h"

#include "render/renderer.h"
#include "render/Scene.h"
#include "render/camera.h"
#include "render/model.h"
#include "render/texture.h"
#include "render/Material.h"
#include "render/shader.h"

#include "valtracker/valtracker.h"

#include "math/matrix.h"

#include "utils/asset.h"
#include "utils/AssetFactory.h"
#include "utils/AssetFile.h"

#include "utils/ResourceManager.h"

#include "generator/TextureLoader.h"
#include "generator/ShaderLoader.h"
#include "generator/MusicBassLoader.h"

using namespace Grafkit;

#include "builtin_data/cube.h"

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
	}

	~Application() {
	}

protected:
	Renderer render;
	Ref<Scene> scene;

	MusicResRef music;
	Timer* timer;
	ValueTracker *valTracker;

	ValueTracker::Float3Track *trk_rotate_root;
	ValueTracker::Float2Track *trk_rotate[10];
	
	TextureSamplerRef m_textureSampler;

	ActorRef m_rootActor;

	ActorRef cameraActor;
	ActorRef modelActor;
	ActorRef modelActors[10];

	float t;

	ShaderResRef m_vertexShader;
	ShaderResRef m_fragmentShader;

	int init() {
		// --- ezeket kell osszeszedni egy initwindowban
		const int screenWidth = m_window.getRealWidth(), screenHeight = m_window.getRealHeight();
		const int VSYNC_ENABLED = 1, FULL_SCREEN = 0;

		this->render.Initialize(screenWidth, screenHeight, VSYNC_ENABLED, this->m_window.getHWnd(), FULL_SCREEN);

		// init file loader
		this->m_file_loader = new FileAssetFactory("./../../assets/rotating_cube/");

		// --------------------------------------------------

		// -- camera
		CameraRef camera = new Camera;
		camera->SetPosition(0.0f, 0.0f, -10.0f);
		camera->SetLookAt(0, 0, 0);

		// -- texture
		TextureResRef texture = new TextureRes();

		texture = this->Load<TextureRes>(new TextureFromBitmap("Normap.jpg"));

		// -- texture sampler
		m_textureSampler = new TextureSampler();
		m_textureSampler->Initialize(render);

		// -- load shader
		m_vertexShader = Load<ShaderRes>(new ShaderLoader("vShader", "texture.hlsl", "TextureVertexShader", ST_Vertex));
		m_fragmentShader = Load<ShaderRes>(new ShaderLoader("pShader", "texture.hlsl", "TexturePixelShader", ST_Pixel));

		// -- load music
		music = Load<MusicRes>(new MusicBassLoader("AceMan - Go Back To The River!.mp3"));

		// -- precalc
		this->DoPrecalc();

		// -- precalc utan egybol allitsuk be a synctrackert 
		// Setup syctracker
		timer = new Timer();
		timer->Initialize(music, music->Get()->GetLengthms(), 180, 8);
		timer->Connect();
		valTracker = new ValueTracker(timer);

		// -- model 
		ModelRef model = new Model;
		model->SetMaterial(new BaseMaterial());
		model->GetMaterial()->AddTexture(texture, "diffuse");


		SimpleMeshGenerator generator(render, m_vertexShader);
		generator["POSITION"] = (void*)GrafkitData::cubeVertices;
		generator["TEXCOORD"] = (void*)GrafkitData::cubeTextureUVs;

		generator(GrafkitData::cubeVertexLength, GrafkitData::cubeIndicesLength, GrafkitData::cubeIndices, model);

		// -- setup scene 
		scene = new Scene();
		this->cameraActor = new Actor(); cameraActor->AddEntity(camera);
		this->modelActor = new Actor(); modelActor->AddEntity(model);
		
		/*
		Alap right-handed koordinatarendszer szerint osszerakunk egy keresztet
		Ezeket adjuk hozza a belso kockahoz
		*/
		float3 cica[] = {
			{ 1, 0, 0 }, /* Jobb */{ -1, 0, 0 }, /* Bal */
			{ 0, 1, 0 }, /* fent */{ 0,-1, 0 }, /* lent */
			{ 0, 0, -1 }, /* elol */{ 0, 0, 1 }, /* hatul */
		};

		char names[] = "r\0l\0u\0d\0f\0b";	/*right, left, up, down, front, back*/

		this->trk_rotate_root = valTracker->newFloat3Track("root", "r", "rpy");

		

		// size_t i = 5; 
		for (size_t i = 0; i < 6; i++)
		{
			ActorRef actor = new Actor();
			actor->AddEntity(model);
			modelActor->AddChild(actor);
			modelActors[i] = actor;

			float3 v = cica[i];
			v.x *= 3;
			v.y *= 3;
			v.z *= 3;

			actor->Matrix().Translate(v);

			this->trk_rotate[i] = valTracker->newFloat2Track("b", &names[2*i], "rpy");

		}


		// ActorRef lightActor = new Actor(); lightActor->AddEntity(light);

		ActorRef rootActor = new Actor();
		// rootActor->AddChild(cameraActor);
		rootActor->AddChild(modelActor);

		m_rootActor = rootActor;

		scene->Initialize(rootActor);
		scene->AddCameraNode(cameraActor);
		// scene->AddLightNode(lightActor);

		scene->SetVShader(m_vertexShader);
		scene->SetFShader(m_fragmentShader);

		// --- 

		this->t = 0;

		return 0;
	};

	// ==================================================================================================================

	void release() {
		this->render.Shutdown();
		delete valTracker;
		delete timer;
	};

	// ==================================================================================================================
	int mainloop() {
		this->render.BeginScene();
		{
			this->t = timer->GetTime();

			ShaderRef fragmentShader = this->m_fragmentShader->Get();

			float2 rot_rp;
			float3 rot_rpy;
			
			rot_rpy = trk_rotate_root->Get(); m_rootActor->Matrix().Identity(); m_rootActor->Matrix().RotateRPY(rot_rpy.x, rot_rpy.y, rot_rpy.z);


			for (int i = 0; i < 6; i++) {
				rot_rp = trk_rotate[i]->Get(); 
				modelActors[i]->Transform().Identity(); 
				modelActors[i]->Transform().RotateRPY(rot_rp.x, rot_rp.y, 0);
			}

			fragmentShader->GetBRes("SampleType").Set(m_textureSampler->GetSamplerState());

			scene->PreRender(render);
			scene->Render(render);

			timer->Update();

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
