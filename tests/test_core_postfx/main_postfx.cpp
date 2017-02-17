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

#include "generator/SceneLoader.h"

#include "generator/TextureLoader.h"
#include "generator/ShaderLoader.h"

#include "core/ResourcePreloader.h"


using namespace Grafkit;

class Application : public System, public ResourcePreloader
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

			screenWidth = m_window.getRealWidth(), screenHeight = m_window.getRealHeight();
			const int VSYNC_ENABLED = 1, FULL_SCREEN = 0;

			this->render.Initialize(screenWidth, screenHeight, VSYNC_ENABLED, this->m_window.getHWnd(), FULL_SCREEN);

			this->m_file_loader = new FileAssetFactory("./../assets/");
		}
		
		~Application() {
		}

protected:
		Renderer render;
		SceneResRef m_scene;

		TextureSamplerRef m_textureSampler;
		ActorRef m_rootActor;

		EffectComposerRef m_postfx;
		
		ShaderResRef m_fxFXAA;
		ShaderResRef m_fxFishEye;
		ShaderResRef m_fxVhs;

		float t;

		ShaderResRef m_vs;
		ShaderResRef m_fs;
		
		int init() {
			m_vs = Load<ShaderRes>(new VertexShaderLoader("vShader", "shaders/default.hlsl", ""));
			m_fs = Load<ShaderRes>(new PixelShaderLoader("pShader", "shaders/default.hlsl", ""));

			m_fxFXAA = Load<ShaderRes>(new PixelShaderLoader("xFXAA", "shaders/fxaa.hlsl", "FXAA"));
			m_fxFishEye = Load<ShaderRes>(new PixelShaderLoader("xFishEye", "shaders/fisheye.hlsl", "fisheyeProc"));

			m_scene = this->Load<SceneRes>(new SceneLoader("scene", "hello.scene"));

			DoPrecalc();

			m_scene->Get()->BuildScene(render, m_vs, m_fs);
			m_rootActor = m_scene->Get()->GetRootNode();
			m_scene->Get()->SetActiveCamera(0);

			// -- setup postfx 
			size_t k = 0;
			m_postfx = new EffectComposer(); 
			m_postfx->Initialize(render);
			
			k = m_postfx->AddPass(new EffectPass()); m_postfx->GetEffect(k)->Initialize(render, m_fxFXAA);
			k = m_postfx->AddPass(new EffectPass()); m_postfx->GetEffect(k)->Initialize(render, m_fxFishEye);
			// k = m_postfx->AddPass(new EffectPass()); m_postfx->GetEffect(k)->Initialize(render, m_fxVhs);
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

			//m_postfx->BindInput(render);

			// pre fx-pass
			this->render.BeginScene();
			{
				m_rootActor->Matrix().Identity();
				m_rootActor->Matrix().RotateRPY(t,0,0);

				//fragmentShader->GetBRes("SampleType").Set(m_textureSampler->GetSamplerState());
				
				float2 res = float2();
				render.GetScreenSizef(res.x, res.y);

				m_fxFXAA->Get()->SetParamValueT<float2>(render, "FXAA", "resolution", res);
				m_fxFishEye->Get()->SetParamValueT<float>(render, "Fisheye", "theta", .5);

				m_scene->Get()->PreRender(render);
				m_scene->Get()->Render(render);

				this->t += 0.01;
			}

			// render fx chain 
			//m_postfx->Render(render);
			
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
