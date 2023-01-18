#include "core/system.h"

#include "render/renderer.h"
#include "render/camera.h"
#include "render/model.h"
#include "render/texture.h"

#include "math/matrix.h"

using namespace FWrender;
using FWmath::Matrix;

#include "textureShaderClass.h"
#include "builtin_data/cube.h"

class Application : public FWcore::System
{  
public:
		Application() : FWcore::System()
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
		CameraRef camera;
		ModelRef model;

		float t;

		TextureShaderClass *shader_texture;
		
		int init() {
			//this->render = new Renderer();

			// --- ezeket kell osszeszedni egy initwindowban
			const int screenWidth = m_window.getRealWidth(), screenHeight = m_window.getRealHeight();
			const int VSYNC_ENABLED = 1, FULL_SCREEN = 0;
			const float SCREEN_DEPTH = 0.1f, SCREEN_NEAR = 1000.f;

			int result = 0;

			result = this->render.Initialize(screenWidth, screenHeight, VSYNC_ENABLED, this->m_window.getHWnd(), FULL_SCREEN);

			// -- camera
			camera = new Camera;
			camera->SetPosition(0.0f, 0.0f, -5.0f);

			// -- texture
			TextureRef texture = new Texture();
			texture->Initialize(render, L"Normap.jpg");

			// -- model 
			model = new Model;
			result = this->model->Initialize(
				render, texture,
				FWBuiltInData::cubeIndicesLength, FWBuiltInData::cubeIndices,
				FWBuiltInData::cubeVertexLength, (float3*)FWBuiltInData::cubeVertices, (float2*)FWBuiltInData::cubeTextureUVs);

			shader_texture = new TextureShaderClass();
			result = this->shader_texture->Initialize(render.GetDevice(), this->m_window.getHWnd());

			this->t = 0;

			return 0;
		};
		
		void release() {
			this->render.Shutdown();
			
			
			//delete this->camera;
			//delete this->model;  // this sucks so hard 
			delete this->shader_texture;

		};
		
		int mainloop() {
			this->render.BeginScene();
			{

				Matrix worldMatrix, viewMatrix, projectionMatrix;

				camera->Render();

				render.GetWorldMatrix(worldMatrix);
				camera->GetViewMatrix(viewMatrix);
				camera->GetProjectionMatrix(projectionMatrix);

				model->Render(render.GetDeviceContext());

				worldMatrix.RotateRPY(t*10, t*15, t*20);

				shader_texture->Render(
					render.GetDeviceContext(),
					model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
					model->GetTexture()
					);

				this->t += 0.001;
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
