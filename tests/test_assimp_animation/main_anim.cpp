#define _USE_MATH_DEFINES
#include <cmath>

#include "core/system.h"

#include "render/renderer.h"
#include "render/camera.h"
#include "render/Material.h"
#include "render/shader.h"

#include "generator/ShaderLoader.h"

#include "generator/SceneLoader.h"
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
		m_vs = Load<ShaderRes>(new VertexShaderLoader("vShader", "shaders/default.hlsl", ""));
		m_fs = Load<ShaderRes>(new PixelShaderLoader("pShader", "shaders/default.hlsl", ""));

		// -- model 
		m_scene = this->Load<SceneRes>(new SceneLoader("scene", "hello.scene"));

		DoPrecalc();
		
		m_scene->Get()->BuildScene(render, m_vs, m_fs);
		m_scene->Get()->SetActiveCamera(0);

		// -- add anim tracks
		ActorAnimation *cameraAnimation = new ActorAnimation();
		cameraAnimation->AddPositionKey(0, float3(0,0,-10));
		
		float a = M_1_PI / 2;

		cameraAnimation->AddRotationKey(0.0, Quaternion::fromEuler(0, 0, 0));
		cameraAnimation->AddRotationKey(1.0, Quaternion::fromEuler(a, 0, 0));
		cameraAnimation->AddRotationKey(2.0, Quaternion::fromEuler(a, a, 0));

		cameraAnimation->AddRotationKey(3.0, Quaternion::fromEuler(a, a, a));
		cameraAnimation->AddRotationKey(4.0, Quaternion::fromEuler(0, a, a));
		cameraAnimation->AddRotationKey(5.0, Quaternion::fromEuler(0, 0, a));
		cameraAnimation->AddRotationKey(6.0, Quaternion::fromEuler(0, 0, 0));
		
		cameraAnimation->SetActor(m_scene->Get()->GetActiveCamera());

		m_scene->Get()->AddAnimation(cameraAnimation);

		// -- save and reload
		SceneLoader::Save(m_scene->Get(), "./../assets/animation.scene");
		m_scene->Get()->Shutdown();
		m_scene = this->Load<SceneRes>(new SceneLoader("AnimatedScene", "animation.scene"));
		DoPrecalc();

		m_scene->Get()->BuildScene(render, m_vs, m_fs);

		m_scene->Get()->SetActiveCamera(0);

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

			this->m_scene->Get()->UpdateAnimation(fmod(m_t, 10.5));
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
