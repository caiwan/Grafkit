#include "core/system.h"

#include "render/renderer.h"
#include "render/Scene.h"
#include "render/camera.h"
#include "render/model.h"
#include "render/texture.h"
#include "render/Material.h"
#include "render/shader.h"

#include "math/matrix.h"

#include "utils/asset.h"
#include "utils/AssetFactory.h"
#include "utils/AssetFile.h"
		  
#include "utils/ResourceManager.h"

#include "generator/SceneLoader.h"

#include "generator/TextureLoader.h"
#include "generator/ShaderLoader.h"

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

		TextureSamplerRef m_textureSampler;

		ActorRef m_rootActor;
		ActorRef m_cameraActor;

		float t;

		ShaderResRef m_vertexShader;
		ShaderResRef m_fragmentShader;
		
		int init() {
			// --- ezeket kell osszeszedni egy initwindowban
			const int screenWidth = m_window.getRealWidth(), screenHeight = m_window.getRealHeight();
			const int VSYNC_ENABLED = 1, FULL_SCREEN = 0;

			this->render.Initialize(screenWidth, screenHeight, VSYNC_ENABLED, this->m_window.getHWnd(), FULL_SCREEN);

			// init file loader
			this->m_file_loader = new FileAssetFactory("./../assets/");

			// --------------------------------------------------

			/* Itt letrhozzuk a kamerat, hozzaadjuk a scenehez, de nem kapcsoljuk hozza a scenegraphoz. */

			// -- camera
			/* Az alap kamera origoban van, +z iranyba nez, es +y felfele irany */
			CameraRef camera = new Camera;

			// -- texture
			TextureResRef texture = new TextureRes();

			texture = this->Load<TextureRes>(new TextureFromBitmap("textures/Untitled.png"));

			// -- texture sampler
			m_textureSampler = new TextureSampler();
			m_textureSampler->Initialize(render);

			// -- load shader
			m_vertexShader = Load<ShaderRes>(new VertexShaderLoader("vShader", "shaders/default.hlsl", ""));
			m_fragmentShader = Load<ShaderRes>(new PixelShaderLoader("pShader", "shaders/default.hlsl", ""));

			// -- precalc
			this->DoPrecalc();

			// -- model 
			ModelRef model = new Model(new Mesh());
			model->SetMaterial(new Material(Material::MT_flat));
			model->GetMaterial()->AddTexture(texture, Material::TT_diffuse);

			model->SetName("cube");
			model->GetMesh()->AddPointer("POSITION", sizeof(GrafkitData::cubeVertices[0]) * 4 * GrafkitData::cubeVertexLength, GrafkitData::cubeVertices);
			model->GetMesh()->AddPointer("TEXCOORD", sizeof(GrafkitData::cubeTextureUVs[0]) * 4 * GrafkitData::cubeVertexLength, GrafkitData::cubeTextureUVs);
			model->GetMesh()->SetIndices(GrafkitData::cubeVertexLength, GrafkitData::cubeIndicesLength, GrafkitData::cubeIndices);
			model->GetMesh()->Build(render, m_vertexShader);

			// -- setup scene 
			scene = new Scene();
			
			m_cameraActor = new Actor(); 
			m_cameraActor->AddEntity(camera);
			
			/* Kocka kozepen */
			ActorRef modelActor = new Actor(); 
			modelActor->AddEntity(model);
			
			/*
			Alap right-handed koordinatarendszer szerint osszerakunk egy keresztet
			Ezeket adjuk hozza a belso kockahoz
			*/
			float3 cube_translation_coords[] = {
				{ 1, 0, 0 }, /* Jobb */ { -1, 0, 0 }, /* Bal */
				{ 0, 1, 0 }, /* fent */ {  0,-1, 0 }, /* lent */
				{ 0, 0, -1}, /* elol */ {  0, 0, 1 }, /* hatul */
			};

			//size_t i = 0; // egyesevel itt lehet hozzaadni/elvenni
			for (size_t i = 0; i < 6; i++)
			{
				ActorRef actor = new Actor();
				actor->AddEntity(model);
				modelActor->AddChild(actor);

				float3 v = cube_translation_coords[i];
				v.x *= 3;
				v.y *= 3;
				v.z *= 3;
				
				actor->Matrix().Translate(v);
				
			}

			// kozepso kockat elrejtjuk
			modelActor->Hide();

			/*
			Kockak felfuzese a rootba
			*/
			m_rootActor = new Actor();
			m_rootActor->AddChild(m_cameraActor);
			m_rootActor->AddChild(modelActor);

			scene->Initialize(m_rootActor);

			scene->SetActiveCamera(0);

			/* Export and import stuff to file, then buiild it */
			SceneLoader::Save(scene, "./../assets/hello.scene");
			scene = this->Load<Scene>(new SceneLoader("scene", "hello.scene"));
			this->DoPrecalc();

			scene->BuildScene(render, m_vertexShader, m_fragmentShader);

			// add shaders
			scene->SetVShader(m_vertexShader);
			scene->SetPShader(m_fragmentShader);

			m_cameraActor->Matrix().Identity();
			m_cameraActor->Matrix().Translate(0,0,-10);


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
				ShaderRef fragmentShader = this->m_fragmentShader->Get();

				m_rootActor->Matrix().Identity();
				m_rootActor->Matrix().RotateRPY(t,0,0);
		
				float f = abs(sin(t));
				m_cameraActor->Transform().Identity();
				m_cameraActor->Transform().Translate(0,0,f);

				scene->PreRender(render);
				scene->Render(render);

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
