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

#include "utils/asset.h"
#include "utils/AssetFactory.h"
#include "utils/AssetFile.h"
		  
#include "utils/ResourceManager.h"

#include "generator/TextureLoader.h"
#include "generator/ShaderLoader.h"


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
		
		ShaderResRef m_fxFXAA;
		ShaderResRef m_fxFishEye;
		ShaderResRef m_fxVhs;

		float t;

		ShaderResRef m_vertexShader;
		ShaderResRef m_fragmentShader;
		
		int init() {
			// --- ezeket kell osszeszedni egy initwindowban
			const int screenWidth = m_window.getRealWidth(), screenHeight = m_window.getRealHeight();
			const int VSYNC_ENABLED = 1, FULL_SCREEN = 0;

			this->render.Initialize(screenWidth, screenHeight, VSYNC_ENABLED, this->m_window.getHWnd(), FULL_SCREEN);

			// init file loader
			this->m_file_loader = new FileAssetManager("./../../assets/postfx/");

			// --------------------------------------------------

			// -- camera
			CameraRef camera = new Camera;
			camera->SetPosition(0.0f, 0.0f, -10.0f);

			// -- texture
			TextureResRef texture = new TextureRes();

			texture = this->Load<TextureRes>(new TextureFromBitmap("Normap.jpg"));

			// -- texture sampler
			m_textureSampler = new TextureSampler();
			m_textureSampler->Initialize(render);

			// -- load shader
			m_vertexShader = Load<ShaderRes>(new ShaderLoader("vShader", "texture.hlsl", "TextureVertexShader", ST_Vertex));
			m_fragmentShader = Load<ShaderRes>(new ShaderLoader("pShader", "texture.hlsl", "TexturePixelShader", ST_Pixel));

			m_fxFXAA = Load<ShaderRes>(new ShaderLoader("xFXAA", "fxaa.hlsl", "FXAA", ST_Pixel));
			m_fxFishEye = Load<ShaderRes>(new ShaderLoader("xFishEye", "fisheye.hlsl", "TextureVertexShader", ST_Pixel));
			m_fxVhs = Load<ShaderRes>(new ShaderLoader("xVhs", "vhstape.hlsl", "TextureVertexShader", ST_Pixel));

			// 
			this->DoPrecalc();

			// -- model 
			ModelRef model = new Model;
			model->SetMaterial(new MaterialBase);
			model->GetMaterial()->AddTexture(texture, "diffuse");


			SimpleMeshGenerator generator(render, m_vertexShader);
			generator["POSITION"] = (void*)GrafkitData::cubeVertices;
			generator["TEXCOORD"] = (void*)GrafkitData::cubeTextureUVs;
			
			generator(GrafkitData::cubeVertexLength, GrafkitData::cubeIndicesLength, GrafkitData::cubeIndices, model);

			// -- setup scene 
			scene = new Scene();
			ActorRef cameraActor = new Actor(); cameraActor->AddEntity(camera);
			ActorRef modelActor = new Actor(); modelActor->AddEntity(model);
			
			ActorRef modelActorL = new Actor(); modelActorL->AddEntity(model); modelActorL->Matrix().Translate(3, 0, 0); modelActor->AddChild(modelActorL);
			ActorRef modelActorR = new Actor(); modelActorR->AddEntity(model); modelActorR->Matrix().Translate(-3, 0, 0); modelActor->AddChild(modelActorR);

			ActorRef modelActorU = new Actor(); modelActorU->AddEntity(model);  modelActorU->Matrix().Translate(0, 3, 0); modelActor->AddChild(modelActorU);
			ActorRef modelActorD = new Actor(); modelActorD->AddEntity(model);  modelActorD->Matrix().Translate(0, -3, 0); modelActor->AddChild(modelActorD);

			ActorRef modelActorF = new Actor(); modelActorF->AddEntity(model); modelActorF->Matrix().Translate(0, 0, 3); modelActor->AddChild(modelActorF);
			ActorRef modelActorB = new Actor(); modelActorB->AddEntity(model); modelActorB->Matrix().Translate(0, 0, -3); modelActor->AddChild(modelActorB);
			

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
			size_t k = 0;
			m_postfx = new EffectComposer(); m_postfx->Initialize(render);
			
			k = m_postfx->AddPass(new EffectPass()); m_postfx->GetEffect(k)->Initialize(render, m_fxFXAA);
			k = m_postfx->AddPass(new EffectPass()); m_postfx->GetEffect(k)->Initialize(render, m_fxFishEye);
			k = m_postfx->AddPass(new EffectPass()); m_postfx->GetEffect(k)->Initialize(render, m_fxVhs);
			// k = m_postfx->AddPass(new EffectPass()); m_postfx->GetEffect(k)->Initialize(render, sahder);
			// k = m_postfx->AddPass(new EffectPass()); m_postfx->GetEffect(k)->Initialize(render, sahder);
			// k = m_postfx->AddPass(new EffectPass()); m_postfx->GetEffect(k)->Initialize(render, sahder);

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
				ShaderRef fragmentShader = this->m_fragmentShader->Get();

				m_rootActor->Matrix().Identity();
				m_rootActor->Matrix().RotateRPY(t,0,0);

				fragmentShader->GetBRes("SampleType").Set(m_textureSampler->GetSamplerState());

				scene->PreRender(render);
				scene->Render(render);

				this->t += 0.01;
			}
			this->render.EndScene();

			// render fx chain 
			m_postfx->Render(render);

			return 0;
		};
	
	private:
		FileAssetManager *m_file_loader;

	public:
		IAssetFactory *GetAssetFactory() { return m_file_loader; }
		Renderer & GetDeviceContext() { return this->render; }
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	Application app;
	return app.execute();
}
