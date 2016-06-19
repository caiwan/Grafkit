#include "core/system.h"

#include "render/renderer.h"
#include "render/camera.h"
#include "render/Material.h"
#include "render/shader.h"

#include "generator/ShaderLoader.h"

#include "generator/assimploader.h"
#include "render/Scene.h"

#include "math/matrix.h"

#include "utils/AssetFile.h"
#include "utils/ResourceManager.h"
#include "core/ResourcePreloader.h"

using namespace Grafkit;

class Application : public Grafkit::System, protected Grafkit::AssetPreloader
{
public:
	Application() : Grafkit::System(),
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
	CameraRef camera;
	
	SceneResRef scene;

	float t;

	ShaderResRef m_vertexShader;
	ShaderResRef m_fragmentShader;

	int init() {
		//this->render = new Renderer();

		// --- ezeket kell osszeszedni egy initwindowban
		const int screenWidth = m_window.getRealWidth(), screenHeight = m_window.getRealHeight();
		const int VSYNC_ENABLED = 1, FULL_SCREEN = 0;

		int result = 0;

		result = this->render.Initialize(screenWidth, screenHeight, VSYNC_ENABLED, this->m_window.getHWnd(), FULL_SCREEN);

		// init file loader
		this->m_file_loader = new Grafkit::FileAssetManager("./");

		LoadCache();

		// -- camera
		camera = new Camera();
		// camera->SetPosition(0.0f, 30.0f, -150.0f);
		// camera->SetClippingPlanes(30, 3000);
		camera->SetName("picsatengely");

		// -- load shader
		m_vertexShader = Load<ShaderRes>(new ShaderLoader("vShader", "shaders/texture.hlsl", "TextureVertexShader", ST_Vertex));
		m_fragmentShader = Load<ShaderRes>(new ShaderLoader("pShader", "shaders/texture.hlsl", "TexturePixelShader", ST_Pixel));

		// -- model 
		scene = this->Load<SceneRes>(new AssimpLoader("models/kockak.dae", m_vertexShader));

		this->t = 0;

		DoPrecalc();

		ActorRef camera_actor = new Actor;
		camera_actor->AddEntity(camera);
		scene->Get()->GetRootNode()->AddChild(camera_actor);
		
		camera_actor->SetName("picsatengely_szinesz");
		scene->Get()->AddCameraNode(camera_actor);

		scene->Get()->SetActiveCamera(scene->Get()->GetCameraCount() - 1);

		scene->Get()->SetVShader(m_vertexShader);
		scene->Get()->SetFShader(m_fragmentShader);

		return 0;
	};

	void release() {
		SaveCache();
		this->render.Shutdown();
	};

	int mainloop() {
		this->render.BeginScene();
		{
			(*this->scene)->PreRender(render);
			(*this->scene)->Render(render);
		}

		this->render.EndScene();
		return 0;
	};

private:
	IAssetFactory *m_file_loader;
public:
	IAssetFactory *GetAssetFactory() { return m_file_loader; }
	Renderer & GetDeviceContext() { return render; }
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	Application app;
	return app.execute();
}
