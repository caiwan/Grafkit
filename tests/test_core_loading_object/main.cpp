#include "core/system.h"

#include "render/renderer.h"
#include "render/camera.h"
#include "render/Material.h"
#include "render/shader.h"

#include "render/Scenegraph.h"

#include "math/matrix.h"

#include "core/renderassets.h"
#include "core/assets.h"

using namespace FWrender;
using FWmath::Matrix;

// #include "textureShaderClass.h"
#include "builtin_data/cube.h"

class Application : public FWcore::System, FWassets::IRenderAssetManager
{
public:
	Application() : FWcore::System(),
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
	//ModelRef model;

	Scenegraph *scene;

	float t;

	// TextureShaderClass *shader_texture;
	ShaderRef shader_vs;
	ShaderRef shader_fs;

	int init() {
		//this->render = new Renderer();

		// --- ezeket kell osszeszedni egy initwindowban
		const int screenWidth = m_window.getRealWidth(), screenHeight = m_window.getRealHeight();
		const int VSYNC_ENABLED = 1, FULL_SCREEN = 0;
		const float SCREEN_DEPTH = 0.1f, SCREEN_NEAR = 1000.f;

		int result = 0;

		result = this->render.Initialize(screenWidth, screenHeight, VSYNC_ENABLED, this->m_window.getHWnd(), FULL_SCREEN);

		// init file loader
		this->m_file_loader = new FWassets::FileResourceManager("./");

		// -- camera
		camera = new Camera;
		camera->SetPosition(0.0f, 0.0f, -5.0f);

		
		scene = new Scenegraph();

		// -- load shader
		shader_vs = new Shader();
		shader_vs->LoadFromFile(render, "TextureVertexShader", L"./texture.hlsl", ST_Vertex);

		shader_fs = new Shader();
		shader_fs->LoadFromFile(render, "TexturePixelShader", L"./texture.hlsl", ST_Pixel);

		// -- model 

		// ide kell majd egy update model, minden materialra

		//model = new Model;
		//model->SetMaterial(new MaterialBase);
		//model->GetMaterial()->AddTexture(texture);	// elobb a texturakat toltod fel, aztad adod hozza a shadert.
		//model->GetMaterial()->SetShader(shader_fs);

		this->t = 0;

		return 0;
	};

	void release() {
		this->render.Shutdown();
		delete this->scene;
	};

	int mainloop() {
		this->render.BeginScene();
		{
			struct {
				matrix worldMatrix;
				matrix viewMatrix;
				matrix projectionMatrix;
			} matbuff;

			camera->Render(render);

			//render.GetWorldMatrix(matbuff.worldMatrix);
			camera->GetViewMatrix(matbuff.viewMatrix);
			camera->GetProjectionMatrix(matbuff.projectionMatrix);

			//matbuff.worldMatrix = DirectX::XMMatrixIdentity(); 
			matbuff.worldMatrix = DirectX::XMMatrixRotationRollPitchYaw(t * 10, t * 15, t * 20);

			// --- ez a legfontosabb dolog, amit meg meg kell itt tenni mielott atadod a cbuffernek:
			matbuff.worldMatrix = XMMatrixTranspose(matbuff.worldMatrix);
			matbuff.viewMatrix = XMMatrixTranspose(matbuff.viewMatrix);
			matbuff.projectionMatrix = XMMatrixTranspose(matbuff.projectionMatrix);
			// ---

			shader_vs["MatrixBuffer"].Set(&matbuff);

			shader_vs->Render(render);
			shader_fs->Render(render);

			model->Render(render);

			this->t += 0.001;
		}

		this->render.EndScene();
		return 0;
	};

private:
	FWassets::FileResourceManager *m_file_loader;
public:
	FWassets::IResourceFactory* GetResourceFactory() { return m_file_loader; };
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	Application app;
	return app.execute();
}
