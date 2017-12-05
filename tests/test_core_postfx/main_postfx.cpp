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
#include "utils/ResourcePreloader.h"
#include "utils/InitializeSerializer.h"

#include "generator/SceneLoader.h"

#include "generator/TextureLoader.h"
#include "generator/ShaderLoader.h"

using namespace Grafkit;

#include "builtin_data/cube.h"


class Application : public System, protected ResourcePreloader, protected ClonableInitializer
{  
public:
	Application() : System(), ClonableInitializer(),
		m_file_loader(nullptr)
		{
			int screenWidth, screenHeight;

			screenWidth = 800;
			screenHeight = 600;

			t = 0;

			// initwindowot is ertelmesebb helyre kell rakni
			InitializeWindows(screenWidth, screenHeight);

			screenWidth = m_window.getRealWidth(), screenHeight = m_window.getRealHeight();
			const int VSYNC_ENABLED = 1, FULL_SCREEN = 0;

			this->render.Initialize(screenWidth, screenHeight, VSYNC_ENABLED, this->m_window.getHWnd(), FULL_SCREEN);

			this->m_file_loader = new FileAssetFactory("./../assets/");
		}
		
		~Application() {
		}

protected:
		Renderer render;
		SceneResRef scene;
		TextureResRef texture;

		TextureSamplerRef sampler;
		ActorRef m_rootActor;

		LightRef light;
		ActorRef lightActor;

		ActorRef cameraActor;
		CameraRef camera;

		EffectComposerRef m_postfx;
		
		ShaderResRef m_fxFXAA;
		ShaderResRef m_fxFishEye;
		ShaderResRef m_fxVhs;

		float t;

		ShaderResRef m_vs;
		ShaderResRef m_fs;
		
		int init() {
			m_vs = Load<ShaderRes>(new VertexShaderLoader("vShader", "shaders/vertex.hlsl", ""));
			m_fs = Load<ShaderRes>(new PixelShaderLoader("pShader", "shaders/flat.hlsl", ""));

			m_fxFXAA = Load<ShaderRes>(new PixelShaderLoader("xFXAA", "shaders/fxaa.hlsl", "FXAA"));
			m_fxFishEye = Load<ShaderRes>(new PixelShaderLoader("xFishEye", "shaders/fisheye.hlsl", "fisheyeProc"));

#if 0
			scene = this->Load<SceneRes>(new SceneLoader("scene", "spheres.scene"));
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
			LoadCache();
			DoPrecalc();

			scene->Get()->BuildScene(render, m_vs, m_fs);
			m_rootActor = (*scene)->GetRootNode();
			(*scene)->SetActiveCamera(0);

			// -- setup postfx 
			size_t k = 0;
			m_postfx = new EffectComposer(); 
		
			m_postfx->AddPass(new EffectPass(m_fxFXAA));
			//m_postfx->AddPass(new EffectPass(m_fxFishEye));
			
			m_postfx->Initialize(render);

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
				m_rootActor->Matrix().RotateRPY(t,0,0);

				//fragmentShader->GetBRes("SampleType").Set(m_textureSampler->GetSamplerState());
				
				float2 res = float2();
				render.GetViewportSizef(res.x, res.y);

				m_fxFXAA->Get()->SetParamValueT<float2>(render, "FXAA", "resolution", res);
				m_fxFishEye->Get()->SetParamValueT<float>(render, "Fisheye", "theta", .1);
				m_fxFishEye->Get()->SetParamValueT<float>(render, "Fisheye", "zoom", 3);

				(*scene)->PreRender(render);
				(*scene)->Render(render);

				this->t += 0.01;
			}

			// render fx chain 
			 m_postfx->Render(render);
			
			this->render.EndScene();

			return 0;
		};
	
	private:
		FileAssetFactory *m_file_loader;

	public:
		IAssetFactory *GetAssetFactory() { return m_file_loader; }
		Renderer & GetDeviceContext() { return this->render; }
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	Application app;
	return app.execute();
}
