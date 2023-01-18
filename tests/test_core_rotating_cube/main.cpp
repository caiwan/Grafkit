#include "core/system.h"

#include "render/renderer.h"
#include "render/Scene.h"
#include "render/camera.h"
#include "render/model.h"
#include "render/texture.h"
#include "render/Material.h"
#include "render/shader.h"

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

			// initwindowot is ertelmesebb helyre kell rakni
			InitializeWindows(screenWidth, screenHeight);
		}
		
		~Application() {
		}

protected:
		Renderer render;
		Ref<Scene> scene;

		float t;

		ShaderRef m_vertexShader;
		ShaderRef m_fragmentShader;
		
		int init() {
			//this->render = new Renderer();

			// --- ezeket kell osszeszedni egy initwindowban
			const int screenWidth = m_window.getRealWidth(), screenHeight = m_window.getRealHeight();
			const int VSYNC_ENABLED = 1, FULL_SCREEN = 0;
			const float SCREEN_DEPTH = 0.1f, SCREEN_NEAR = 1000.f;

			int result = 0;

			result = this->render.Initialize(screenWidth, screenHeight, VSYNC_ENABLED, this->m_window.getHWnd(), FULL_SCREEN);

			// init file loader
			this->m_file_loader = new FileAssetManager("./");

			// --------------------------------------------------

			// -- camera
			CameraRef camera = new Camera;
			camera->SetPosition(0.0f, 0.0f, -5.0f);

			// -- texture
			TextureResRef texture = new TextureRes();

			TextureFromBitmap txgen( m_file_loader->Get("Normap.jpg"), texture );
			txgen(this);

			// -- load shader
			m_vertexShader = new Shader();
			m_vertexShader->LoadFromFile(render, "TextureVertexShader", L"./texture.hlsl", ST_Vertex);

			m_fragmentShader = new Shader();
			m_fragmentShader->LoadFromFile(render, "TexturePixelShader", L"./texture.hlsl", ST_Pixel);

			// -- model 
			ModelRef model = new Model;
			model->SetMaterial(new MaterialBase);
			model->GetMaterial()->AddTexture(texture, "diffuse");


			SimpleMeshGenerator generator(render, m_vertexShader);
			generator["POSITION"] = (void*)FWBuiltInData::cubeVertices;
			generator["TEXCOORD"] = (void*)FWBuiltInData::cubeTextureUVs;
			
			generator(FWBuiltInData::cubeVertexLength, FWBuiltInData::cubeIndicesLength, FWBuiltInData::cubeIndices, model);


			// -- setup scene 
			scene = new Scene();
			ActorRef cameraActor = new Actor(); cameraActor->AddEntity(camera);
			ActorRef modelActor = new Actor(); modelActor->AddEntity(model);
			// ActorRef lightActor = new Actor(); lightActor->AddEntity(light);

			ActorRef rootActor = new Actor();
			rootActor->AddChild(cameraActor);
			rootActor->AddChild(modelActor);

			scene->SetRootNode(rootActor);
			scene->SetCameraNode(cameraActor);
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
		};

		// ==================================================================================================================
		int mainloop() {
			this->render.BeginScene();
			{
				//struct {
				//	matrix worldMatrix;
				//	matrix viewMatrix;
				//	matrix projectionMatrix;
				//} matbuff;

				//camera->Calculate(render);

				//matbuff.viewMatrix = camera->GetViewMatrix().Get();
				//matbuff.projectionMatrix= camera->GetProjectionMatrix().Get();

				////matbuff.worldMatrix = DirectX::XMMatrixIdentity(); 
				//matbuff.worldMatrix = DirectX::XMMatrixRotationRollPitchYaw(t*10, t*15, t*20);

				//// --- ez a legfontosabb dolog, amit meg meg kell itt tenni mielott atadod a cbuffernek:
				//matbuff.worldMatrix = XMMatrixTranspose(matbuff.worldMatrix);
				//matbuff.viewMatrix = XMMatrixTranspose(matbuff.viewMatrix);
				//matbuff.projectionMatrix = XMMatrixTranspose(matbuff.projectionMatrix);
				//// ---

				//shader_vs["MatrixBuffer"].Set(&matbuff);

				//shader_vs->Render(render);
				//shader_fs->Render(render);

				//model->Render(render);
				
				scene->PreRender(render);
				scene->Render(render);

				this->t += 0.001;
			}

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
