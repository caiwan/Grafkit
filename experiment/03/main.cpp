#include "core/system.h"

#include "render/renderer.h"
#include "render/camera.h"
#include "render/model.h"

using FWrender::Renderer;
using FWrender::Camera;
using FWrender::Model;

#include "textureShaderClass.h"

class Application : public FWcore::System
{  
public:
		Application()  : FWcore::System()
		{
			int screenWidth, screenHeight;

			screenWidth = 800;
			screenHeight = 600;

			// initwindowot is ertelmesebb helyre kell rakni
			InitializeWindows(screenWidth, screenHeight);
		}
		
		~Application() {
		}

		Renderer render;
		Camera *camera;
		Model *model;

		TextureShaderClass *shader_texture;
		
		int init() {
			//this->render = new Renderer();

			// --- ezeket kell osszeszedni egy initwindowban
			const int screenWidth = m_window.getRealWidth(), screenHeight = m_window.getRealHeight();
			const int VSYNC_ENABLED = 1, FULL_SCREEN = 0;
			const float SCREEN_DEPTH = 0.1f, SCREEN_NEAR = 1000.f;

			int result = 0;

			result = this->render.Initialize(screenWidth, screenHeight,  VSYNC_ENABLED,  this->m_window.getHWnd(), FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR );

			// -- camera
			camera = new Camera;
			camera->SetPosition(0.0f, 0.0f, -5.0f);

			model = new Model;
			result = this->model->Initialize(render.GetDevice(), L"./Normap.jpg");

			shader_texture = new TextureShaderClass();
			result = this->shader_texture->Initialize(render.GetDevice(), this->m_window.getHWnd());

			return 0;
		};
		
		void release() {
			this->render.Shutdown();
			delete this->camera;
			delete this->model;
			delete this->shader_texture;
			//delete this->render;
		};
		
		int mainloop() {
			this->render.BeginScene();
			{

				XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

				camera->Render();

				render.GetWorldMatrix(worldMatrix);
				camera->GetViewMatrix(viewMatrix);
				render.GetProjectionMatrix(projectionMatrix);

				model->Render(render.GetDeviceContext());

				shader_texture->Render(
					render.GetDeviceContext(),
					model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
					model->GetTexture()
					);
			}
			this->render.EndScene();
			return 0;
		};
	
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	Application app;
	return app.execute();
}