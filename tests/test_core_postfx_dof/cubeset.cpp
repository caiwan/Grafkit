#include "cubeset.h"

#include "generator/ShaderLoader.h"
#include "generator/TextureLoader.h"

#include "builtin_data/cube.h"

#include "render/scene.h"

using namespace Grafkit;

CubeScene::CubeScene()
{
}

CubeScene::~CubeScene()
{
}

void CubeScene::Shutdown()
{
}

void CubeScene::OnBeforePreload(Grafkit::Renderer & render, GKDemo::DemoApplication * const & context, Grafkit::IResourceManager * const & resman)
{
	texture = resman->Load<Texture2DRes>(new TextureFromBitmap("Untitled.png", "textures/Untitled.png"));

	sampler = resman->Load<TextureSamplerRes>(new TextureSamplerBuilder(TextureSamplerBuilder::TGG_Wrapping));

	vs = resman->Load<ShaderRes>(new VertexShaderLoader("vShader", "shaders/vertex.hlsl", ""));
	fs = resman->Load<ShaderRes>(new PixelShaderLoader("pShader", "shaders/flat.hlsl", ""));

#if 0
	scene = this->Load<SceneRes>(new SceneLoader("scene", "ao.scene"));
#endif
}

void CubeScene::OnAfterPreload(Grafkit::Renderer & render, GKDemo::DemoApplication * const & context)
{
#if 1
	camera = new Camera;
	camera->SetName("camera");

	ModelRef model = new Model(GrafkitData::CreateCube());
	model->SetMaterial(new Material());
	model->GetMaterial()->AddTexture(texture, Material::TT_diffuse);
	model->GetMaterial()->SetName("GridMaterial");

	// -- setup scene 
	scene = new SceneRes(new Scene());

	cameraActor = new Actor();
	cameraActor->SetName("cameraNode");
	cameraActor->AddEntity(camera);

	cameraActor->Matrix().LookAtLH(float3(10, 10, -10));

	ActorRef modelActor = new Actor();
	modelActor->SetName("center");

#define N 9
	for (int x = 0; x < N; x++) {
		for (int y = 0; y < N; y++) {
			ActorRef actor = new Actor();
			actor->AddEntity(model);

			float xx = x - N / 2;
			float yy = y - N / 2;
			float zz = (float)(rand() % 256) / 256.;

			actor->Matrix().Scale(.5, .5, .5);
			actor->Matrix().Translate(xx, zz, yy);
			modelActor->AddChild(actor);
		}
	}
#undef N

	ActorRef rootActor;
	rootActor = new Actor();
	rootActor->SetName("root");
	rootActor->AddChild(cameraActor);
	rootActor->AddChild(modelActor);

	(*scene)->Initialize(rootActor);

	(*scene)->BuildScene(render, vs, fs);
	(*scene)->SetActiveCamera(0);

	cameraActor = (*scene)->GetActiveCameraNode();
	camera = (*scene)->GetActiveCamera();

#endif
}

void CubeScene::OnBeforeRender(Grafkit::Renderer & render, GKDemo::DemoApplication * const & context)
{
	(*scene)->UpdateAnimation(0.f);
	(*scene)->PreRender(render);
}

void CubeScene::OnRender(Grafkit::Renderer & render, GKDemo::DemoApplication * const & context)
{
	(*scene)->Render(render);
}
