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

	CameraRef camera;

	TextureSamplerRef texSampler;
	TextureRef texture;
	ModelRef model;

	float t;

	ShaderRef vs;
	ShaderRef fs;

	int init() {
		// --- ezeket kell osszeszedni egy initwindowban
		const int screenWidth = m_window.getRealWidth(), screenHeight = m_window.getRealHeight();
		const int VSYNC_ENABLED = 1, FULL_SCREEN = 0;

		this->render.Initialize(screenWidth, screenHeight, VSYNC_ENABLED, this->m_window.getHWnd(), FULL_SCREEN);

		// init file loader
		this->m_file_loader = new FileAssetFactory("./../assets/");

		// --------------------------------------------------

		// -- camera
		camera = new Camera;
		//camera->SetPosition(0.0f, 0.0f, 10.0f);

		// -- texture

		TextureFromBitmap *textureLoader = new TextureFromBitmap("normap", "textures/normap.jpg");
		TextureResRef textureRes = (Texture2DRes*)textureLoader->NewResource();
		textureLoader->Load(this, textureRes);

		this->texture = textureRes->Get();

		delete textureLoader;

		// -- texture sampler
		texSampler = new TextureSampler();
		texSampler->Initialize(render);

		// -- load shader

		ShaderLoader *vShaderLoader = new VertexShaderLoader("vshader", "shaders/vertex.hlsl", "");
		ShaderResRef vShaderRef = (ShaderRes*)(vShaderLoader->NewResource());
		vShaderLoader->Load(this, vShaderRef);

		vs = vShaderRef->Get();

		delete vShaderLoader;

		ShaderLoader *pShaderLoader = new PixelShaderLoader("pshader", "shaders/flat.hlsl", "");
		ShaderResRef pShaderRef = (ShaderRes*)(pShaderLoader->NewResource());
		pShaderLoader->Load(this, pShaderRef);

		fs = pShaderRef->Get();

		delete pShaderLoader;

		// -- model 
		model = new Model(new Mesh());
		model->SetName("cube");
		model->GetMesh()->AddPointer("POSITION", GrafkitData::cubeVertexSize, GrafkitData::cubeVertices);
		model->GetMesh()->AddPointer("TEXCOORD", GrafkitData::cubeTextureUVsSize, GrafkitData::cubeTextureUVs);
		model->GetMesh()->AddPointer("NORMAL", GrafkitData::cubeVertexSize, GrafkitData::cubeNormals);
		model->GetMesh()->SetIndices(GrafkitData::cubeVertexCount, GrafkitData::cubeIndicesCount, GrafkitData::cubeIndices);
		model->GetMesh()->Build(render, vs);

		// export as test
		FILE* fp = nullptr; fopen_s(&fp, "cube.mesh", "wb");
		ArchiveFile store(fp, true);

		this->model->store(store);

		fflush(fp); fclose(fp);

		// import it again
		fopen_s(&fp, "cube.mesh", "rb");
		ArchiveFile load(fp, false);

		this->model->load(load);
		this->model->GetMesh()->Build(render, vs);

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
		this->render.BeginSceneDev();
		{
			Matrix cameraMatrix;
			camera->Calculate(render);
			cameraMatrix = camera->GetViewMatrix();
			cameraMatrix .Translate(0, 0, 10);

			Matrix worldMatrix;
			worldMatrix.Identity();
			worldMatrix.RotateRPY(t, .3455*t, .7346*t);

			struct {
				matrix worldMatrix;
				matrix viewMatrix;
				matrix projectionMatrix;
			} viewMatrices;


			viewMatrices.worldMatrix = XMMatrixTranspose(worldMatrix.Get());
			viewMatrices.viewMatrix = XMMatrixTranspose(cameraMatrix.Get());
			viewMatrices.projectionMatrix = XMMatrixTranspose(camera->GetPerspectiveMatrix().Get());

			vs->SetParam(render, "MatrixBuffer", &viewMatrices);

			struct {
				float4 mat_diffuse, mat_specular;
			} material;

			material.mat_diffuse = float4(1, 0, 0, 1);

			vs->SetParam(render, "material_colors", &material);


			vs->SetParam(render, "MatrixBuffer", &viewMatrices);


			fs->SetShaderResourceView(render, "diffuse", texture->GetShaderResourceView());
			fs->SetSamplerSatate(render, "SampleType", texSampler->GetSamplerState());

			vs->Bind(this->render);
			fs->Bind(this->render);

			model->Render(this->render, nullptr);

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
