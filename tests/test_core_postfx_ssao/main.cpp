#include "core/system.h"

#include "render/renderer.h"
#include "render/Scene.h"
#include "render/camera.h"
#include "render/model.h"
#include "render/texture.h"
#include "render/Material.h"
#include "render/shader.h"

#include "render/effect.h"

#include "math/matrix.h"

#include "core/asset.h"
#include "core/AssetFactory.h"
#include "core/AssetFile.h"

#include "core/ResourceManager.h"

#include "generator/TextureLoader.h"

using namespace Grafkit;

// #include "textureShaderClass.h"
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

		TextureSamplerRef m_textureSampler;
		ActorRef m_rootActor;

		EffectComposerRef m_postfx;

		float t;

		ShaderRef m_vertexShader, m_fragmentShader;
		
		int init() {
			// --- ezeket kell osszeszedni egy initwindowban
			const int screenWidth = m_window.getRealWidth(), screenHeight = m_window.getRealHeight();
			const int VSYNC_ENABLED = 1, FULL_SCREEN = 0;

			this->render.Initialize(screenWidth, screenHeight, VSYNC_ENABLED, this->m_window.getHWnd(), FULL_SCREEN);

			// init file loader
			this->m_file_loader = new FileAssetManager("./");

			// --------------------------------------------------

			// -- camera
			CameraRef camera = new Camera;
			camera->SetPosition(0.0f, 0.0f, -10.0f);

			// -- load shader
			m_vertexShader = new Shader();
			m_vertexShader->LoadFromFile(render, "TextureVertexShader", L"./flat.hlsl", ST_Vertex);

			m_fragmentShader = new Shader();
			m_fragmentShader->LoadFromFile(render, "TexturePixelShader", L"./flat.hlsl", ST_Pixel);

			// -- model 
			ModelRef model = new Model;
			model->SetMaterial(new MaterialBase);


			SimpleMeshGenerator generator(render, m_vertexShader);
			generator["POSITION"] = (void*)GrafkitData::cubeVertices;
			generator["TEXCOORD"] = (void*)GrafkitData::cubeTextureUVs;
			generator["NORMAL"] = (void*)GrafkitData::cubeNormals;
			
			generator(GrafkitData::cubeVertexLength, GrafkitData::cubeIndicesLength, GrafkitData::cubeIndices, model);

			// -- setup scene 
			scene = new Scene();
			ActorRef cameraActor = new Actor(); cameraActor->AddEntity(camera);
			ActorRef modelActor = new Actor(); modelActor->AddEntity(model);
			
			ActorRef modelActorL = new Actor(); modelActorL->AddEntity(model); modelActorL->Matrix().Translate(2.25, 0, 0); modelActor->AddChild(modelActorL);
			ActorRef modelActorR = new Actor(); modelActorR->AddEntity(model); modelActorR->Matrix().Translate(-2.25, 0, 0); modelActor->AddChild(modelActorR);

			ActorRef modelActorU = new Actor(); modelActorU->AddEntity(model);  modelActorU->Matrix().Translate(0, 2.25, 0); modelActor->AddChild(modelActorU);
			ActorRef modelActorD = new Actor(); modelActorD->AddEntity(model);  modelActorD->Matrix().Translate(0, -2.25, 0); modelActor->AddChild(modelActorD);

			ActorRef modelActorF = new Actor(); modelActorF->AddEntity(model); modelActorF->Matrix().Translate(0, 0, 2.25); modelActor->AddChild(modelActorF);
			ActorRef modelActorB = new Actor(); modelActorB->AddEntity(model); modelActorB->Matrix().Translate(0, 0, -2.25); modelActor->AddChild(modelActorB);
			

			// ActorRef lightActor = new Actor(); lightActor->AddEntity(light);

			ActorRef rootActor = new Actor();
			rootActor->AddChild(cameraActor);
			rootActor->AddChild(modelActor);

			m_rootActor = rootActor;

			scene->SetRootNode(rootActor);
			scene->SetCameraNode(cameraActor);
			// scene->AddLightNode(lightActor);

			scene->SetVShader(m_vertexShader);
			scene->SetFShader(m_fragmentShader);

			// -- setup postfx 

			m_postfx = new EffectComposer();
			m_postfx->Initialize(render);

			ShaderRef ssao_shader = new Shader();
			ssao_shader->LoadFromFile(render, "TexturePixelShader", L"./ssao.hlsl", ST_Pixel);
			m_postfx->AddPass(new EffectPass()); m_postfx[0]->Initialize(render, ssao_shader);



			// --- 

			return 0;
		};
		
		// ==================================================================================================================

		void release() {
			this->render.Shutdown();
		};

		// ==================================================================================================================
		int mainloop() {

			m_postfx->BindInput(render);

			// pre fx-pass
			this->render.BeginScene();
			{
				m_rootActor->Matrix().Identity();
				m_rootActor->Matrix().RotateRPY(t, 2.23234*t, 3.2645*t);
				//m_rootActor->Matrix().RotateRPY(t, 0,0);

				// m_fragmentShader->GetBRes("SampleType") = m_textureSampler->GetSamplerState();

				scene->PreRender(render);
				scene->Render(render);

				this->t += 0.01;
			}

			// render fx chain 
			m_postfx->Render(render);

			this->render.EndScene();

			return 0;
		};
	
	private:
		FileAssetManager *m_file_loader;

	public:
		//FWassets::IResourceFactory* GetResourceFactory() { return m_file_loader; };
		Renderer & GetDeviceContext() { return this->render; };
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	Application app;
	return app.execute();
}
