#define _USE_MATH_DEFINES
#include <cmath>

#include "core/system.h"

#include "render/renderer.h"
#include "render/camera.h"
#include "render/Material.h"
#include "render/shader.h"

#include "generator/ShaderLoader.h"

#include "utils/InitializeSerializer.h"

#include "generator/SceneLoader.h"
#include "render/Scene.h"

#include "math/matrix.h"

#include "utils/AssetFile.h"
#include "utils/ResourceManager.h"
#include "utils/ResourcePreloader.h"

using namespace Grafkit;

class Application : public Grafkit::System, protected Grafkit::ResourcePreloader, protected Grafkit::ClonableInitializer
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
		this->m_file_loader = new FileAssetFactory("./../assets/");
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

	ShaderResRef m_vs;
	ShaderResRef m_fs;

	int init() {
		LoadCache();

		// -- load shader
		m_vs = Load<ShaderRes>(new VertexShaderLoader("vShader", "shaders/vertex.hlsl", ""));
		m_fs = Load<ShaderRes>(new PixelShaderLoader("pShader", "shaders/flat.hlsl", ""));

		// -- model 
		//m_scene = this->Load<SceneRes>(new SceneLoader("scene", "box.scene"));
		//m_scene = this->Load<SceneRes>(new SceneLoader("scene", "locRotScale.scene"));
		m_scene = this->Load<SceneRes>(new SceneLoader("scene", "animtest2.scene"));

		DoPrecalc();

		m_scene->Get()->BuildScene(render, m_vs, m_fs);
		//m_scene->Get()->SetActiveCamera(0);
		m_scene->Get()->SetActiveCamera("MainCamera");

		m_currCameraActor = m_scene->Get()->GetCamera(0);
		Camera * c = dynamic_cast<Camera*>(m_currCameraActor->GetEntities()[0].Get());
		if (c) {
			//c->SetUp(0, 0, 1);
			//c->SetLookTo(0, -1, 0);

			//c->SetLookAt(0, 0, 0);
			//c->SetPosition(10, -10, 10);
			//c->SetLookTo(0, 0, 0);
		}

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

			//m_currCameraActor->Transform().Identity();
			//m_currCameraActor->Matrix().Identity();

			//m_currCameraActor->Matrix().RotateRPY(55. * M_PI / 180., 0 * M_PI / 180., 45. * M_PI / 180.);
			//m_currCameraActor->Matrix().Translate(5, 5, 5);
			//m_currCameraActor->Matrix().Translate(0, 5, 0);

			this->m_scene->Get()->UpdateAnimation(fmod(m_t, 10.5));
			//this->m_scene->Get()->UpdateAnimation(0.);
			this->m_scene->Get()->PreRender(render);
			this->m_scene->Get()->Render(render);

		}

		this->render.EndScene();

		//this->GetAssetFactory()->PollEvents(this);

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
