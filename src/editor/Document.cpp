#include "Document.h"

#include "math/fbm.h"

#include "render/shader.h"

#include "render/mesh.h"
#include "render/model.h"
#include "render/material.h"

#include "render/scene.h"

#include "generator/ShaderLoader.h"

#include "utils/ResourceManager.h"

using namespace Idogep;
using namespace Grafkit;

#include "builtin_data/cube.h"

Idogep::Document::Document()
{
	m_testAnimation = new Animation::FloatTrack();
	for (int i = 0; i < 35; i++) {
		m_testAnimation->AddKey(Animation::FloatKey(i, PseudoRandom::Random()));
	}
}

Idogep::Document::~Document()
{
}

void Idogep::Document::Preload(IResourceManager * const & resman)
{
	m_vs = resman->Load<ShaderRes>(new VertexShaderLoader("vertexShader", "shaders/vertex.hlsl", ""));
	m_ps = resman->Load<ShaderRes>(new PixelShaderLoader("pixelShader", "shaders/flat.hlsl", ""));
}

void Idogep::Document::Initialize(Grafkit::Renderer & render)
{
	// --- TEST STUFF ---

	// -- model 
	ModelRef model = new Model(new Mesh());
	model->SetMaterial(new Material());
	//model->GetMaterial()->AddTexture(texture, Material::TT_diffuse);
	model->GetMaterial()->SetName("GridMaterial");

	CameraRef camera = new Camera();
	camera->SetName("Camera");

	ActorRef cameraActor = new Actor(camera);
	cameraActor->SetName("CameraActor");

	cameraActor->Matrix().Identity();
	cameraActor->Matrix().Translate(0, 0, -10);

	model->SetName("cube");
	model->GetMesh()->AddPointer("POSITION", GrafkitData::cubeVertexSize, GrafkitData::cubeVertices);
	model->GetMesh()->AddPointer("TEXCOORD", GrafkitData::cubeVertexSize, GrafkitData::cubeTextureUVs);
	model->GetMesh()->AddPointer("NORMAL", GrafkitData::cubeVertexSize, GrafkitData::cubeNormals);
	model->GetMesh()->SetIndices(GrafkitData::cubeVertexCount, GrafkitData::cubeIndicesCount, GrafkitData::cubeIndices);
	model->GetMesh()->Build(render, m_vs);

	ActorRef cubeActor = new Actor(model);
	cubeActor->SetName("CubeActor");

	ActorRef rootActor = new Actor();
	rootActor->SetName("RootActor");

	rootActor->AddChild(cameraActor);
	rootActor->AddChild(cubeActor);

	m_scenegraph = new SceneRes(new Scene());

	m_scenegraph->Get()->Initialize(rootActor);

	m_scenegraph->Get()->BuildScene(render, m_vs, m_ps);
	m_scenegraph->Get()->SetActiveCamera(0);

	m_rootActor = m_scenegraph->Get()->GetRootNode();
	m_cameraActor = m_scenegraph->Get()->GetCamera(0);
}

void Idogep::Document::Shutdown()
{
}
