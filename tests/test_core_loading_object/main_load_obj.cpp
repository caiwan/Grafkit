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

using namespace Grafkit;

class Application : public Grafkit::System, public Grafkit::IResourceManager // Grafkit::AssetPreloader
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
		const float SCREEN_DEPTH = 0.1f, SCREEN_NEAR = 1000.f;

		int result = 0;

		result = this->render.Initialize(screenWidth, screenHeight, VSYNC_ENABLED, this->m_window.getHWnd(), FULL_SCREEN);

		// init file loader
		this->m_file_loader = new Grafkit::FileAssetManager("./");

		// -- camera
		camera = new Camera();
		camera->SetPosition(0.0f, 0.0f, -5.0f);

		// -- load shader
		m_vertexShader = Load<ShaderRes>(new ShaderLoader("vShader", "shaders/texture.hlsl", "TextureVertexShader", ST_Vertex));
		m_fragmentShader = Load<ShaderRes>(new ShaderLoader("pShader", "shaders/texture.hlsl", "TexturePixelShader", ST_Pixel));

		// -- model 
		scene = this->Load<SceneRes>(new AssimpLoader("./models/tegla.3ds", m_vertexShader));

		this->t = 0;

		DoPrecalc();

		ActorRef camera_actor = new Actor;
		camera_actor->AddEntity(camera);
		scene->Get()->SetCameraNode(camera_actor);

		scene->Get()->SetVShader(m_vertexShader);
		scene->Get()->SetFShader(m_fragmentShader);

		return 0;
	};

	void release() {
		// this->SaveCache();

		this->render.Shutdown();
	};

	int mainloop() {
		this->render.BeginScene();
		{

			(*this->scene)->Render(render);

			this->t += 0.001;
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
