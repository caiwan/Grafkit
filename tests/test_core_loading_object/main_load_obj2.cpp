#define _USE_MATH_DEFINES
#include <cmath>

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

		//this->render = new Renderer();

		// --- ezeket kell osszeszedni egy initwindowban
		screenWidth = m_window.getRealWidth(), screenHeight = m_window.getRealHeight();
		const int VSYNC_ENABLED = 1, FULL_SCREEN = 0;

		int result = 0;

		result = this->render.Initialize(screenWidth, screenHeight, VSYNC_ENABLED, this->m_window.getHWnd(), FULL_SCREEN);
		this->m_file_loader = new Grafkit::FileAssetFactory("./../assets/");
	}

	~Application() {
		delete m_file_loader;	///TODO: shutdown elotte
		this->render.Shutdown();
	}

protected:
	Renderer render;
	
	ActorRef m_currCameraActor;
	CameraRef m_camera;
	
	SceneResRef m_scene;

	float m_t;

	ShaderResRef m_vertexShader;
	ShaderResRef m_fragmentShader;

	int init() {
		LoadCache();

		// -- load shader
		m_vertexShader = Load<ShaderRes>(new ShaderLoader("vShader", "shaders/default.hlsl", "mainVertex", ST_Vertex));
		m_fragmentShader = Load<ShaderRes>(new ShaderLoader("pShader", "shaders/default.hlsl", "mainPixel", ST_Pixel));

		// -- model 
		m_scene = this->Load<SceneRes>(new AssimpLoader("models/sphere.assbin", m_vertexShader));

		DoPrecalc();

		
		// m_currCameraActor->Matrix().Identity();

		// add new dummy camera
		m_camera = new PerspectiveCamera();
		//camera->SetPosition(7.48, -5.34, -6.5);
		m_camera->SetPosition(-5, 6, 15);
		m_camera->SetLookAt(0, 0, 0);
		ActorRef cameraActor = new Actor(m_camera);

		m_scene->Get()->AddCameraNode(cameraActor);
		m_scene->Get()->SetActiveCamera(0);

		m_currCameraActor = m_scene->Get()->GetActiveCamera();

		m_scene->Get()->SetVShader(m_vertexShader);
		m_scene->Get()->SetFShader(m_fragmentShader);

		m_t = 0;

		return 0;
	};

	void release() {
		SaveCache();
		RemoveAll();
	};

	int mainloop() {
		this->render.BeginScene();
		{
			m_t += .01;

			// m_camera->SetPosition(0, -10, 15*(1.25+sin(m_t)));
			m_currCameraActor->Transform().Identity();
			float f = 5 * sin(m_t);
			m_currCameraActor->Transform().Translate(0, 0, 0);

			(*this->m_scene)->PreRender(render);
			(*this->m_scene)->Render(render);

		}

		this->render.EndScene();

		this->GetAssetFactory()->PollEvents(this);

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
