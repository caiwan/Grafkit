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

#include "generator/TextureLoader.h"
#include "generator/ShaderLoader.h"

#include "utils/persistence/archive.h"

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

		CameraRef m_camera;

		TextureSamplerRef m_textureSampler;
		TextureRef m_texture;
		ModelRef m_model;

		float t;

		ShaderRef m_vertexShader;
		ShaderRef m_fragmentShader;
		
		int init() {
			// --- ezeket kell osszeszedni egy initwindowban
			const int screenWidth = m_window.getRealWidth(), screenHeight = m_window.getRealHeight();
			const int VSYNC_ENABLED = 1, FULL_SCREEN = 0;

			this->render.Initialize(screenWidth, screenHeight, VSYNC_ENABLED, this->m_window.getHWnd(), FULL_SCREEN);

			// init file loader
			this->m_file_loader = new FileAssetFactory("./../../assets/rotating_cube/");

			// --------------------------------------------------

			// -- camera
			m_camera = new Camera;
			m_camera->SetPosition(0.0f, 0.0f, -10.0f);

			// -- texture
			
			TextureFromBitmap *textureLoader = new TextureFromBitmap("normap.jpg");
			TextureResRef textureRes = (TextureRes*)textureLoader->NewResource();
			textureLoader->Load(this, textureRes);

			this->m_texture = textureRes->Get();

			delete textureLoader;

			// -- texture sampler
			m_textureSampler = new TextureSampler();
			m_textureSampler->Initialize(render);

			// -- load shader
			
			ShaderLoader *vShaderLoader = new ShaderLoader("vshader", "texture.hlsl", "TextureVertexShader", ST_Vertex);
			ShaderResRef vShaderRef = (ShaderRes*)(vShaderLoader->NewResource());
			vShaderLoader->Load(this, vShaderRef);
			
			m_vertexShader = vShaderRef->Get();

			delete vShaderLoader;

			ShaderLoader *pShaderLoader = new ShaderLoader("pshader", "texture.hlsl", "TexturePixelShader", ST_Pixel);
			ShaderResRef pShaderRef = (ShaderRes*)(pShaderLoader->NewResource());
			pShaderLoader->Load(this, pShaderRef);

			m_fragmentShader = pShaderRef->Get();

			delete pShaderLoader;

			// -- model 
			this->m_model = new Model(new Mesh());
			this->m_model->SetName("cube");
			this->m_model->GetMesh()->AddPointer("POSITION", sizeof(GrafkitData::cubeVertices[0]) * 4 * GrafkitData::cubeVertexLength, GrafkitData::cubeVertices);
			this->m_model->GetMesh()->AddPointer("TEXCOORD", sizeof(GrafkitData::cubeTextureUVs[0]) * 4 * GrafkitData::cubeVertexLength, GrafkitData::cubeTextureUVs);
			this->m_model->GetMesh()->SetIndices(GrafkitData::cubeVertexLength, GrafkitData::cubeIndicesLength, GrafkitData::cubeIndices);
			this->m_model->GetMesh()->Build(m_vertexShader, render);

			// export as test
			FILE* fp = nullptr; fopen_s(&fp, "cube.gkobj", "wb");
			ArchiveFile store(fp, true);
			
			this->m_model->store(store);

			fflush(fp); fclose(fp);

			// import it again
			fopen_s(&fp, "cube.gkobj", "rb");
			ArchiveFile load(fp, false);

			this->m_model->load(load);
			this->m_model->GetMesh()->Build(m_vertexShader, render);

			fclose(fp);


			// --- 

			this->t = 0;

			return 0;
		};
		
		// ==================================================================================================================

		void release() {
			this->render.Shutdown();
			delete this->m_file_loader; this->m_file_loader = nullptr;
		};

		// ==================================================================================================================
		int mainloop() {
			this->render.BeginScene();
			{		
				Matrix worldMatrix;
				worldMatrix.Identity();
				worldMatrix.RotateRPY(t, .3455*t, .7346*t);

				struct {
					matrix worldMatrix;
					matrix viewMatrix;
					matrix projectionMatrix;
				} viewMatrices;

				m_camera->Calculate(render);

				viewMatrices.worldMatrix = XMMatrixTranspose(worldMatrix.Get());
				viewMatrices.viewMatrix = XMMatrixTranspose(m_camera->GetViewMatrix().Get());
				viewMatrices.projectionMatrix = XMMatrixTranspose(m_camera->GetPerspectiveMatrix().Get());

				m_vertexShader->GetParam("MatrixBuffer").SetP(&viewMatrices);

				m_fragmentShader->GetBRes("diffuse").Set(m_texture->GetTextureResource());
				m_fragmentShader->GetBRes("SampleType").Set(m_textureSampler->GetSamplerState());

				m_vertexShader->Render(this->render);
				m_fragmentShader->Render(this->render);

				m_model->Render(this->render, nullptr);

				this->t += 0.01;
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
