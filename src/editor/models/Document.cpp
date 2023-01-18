#include "Document.h"

#include "math/fbm.h"

#include "render/shader.h"

#include "render/mesh.h"
#include "render/model.h"
#include "render/material.h"

#include "render/SceneGraph.h"
#include "animation/scene.h"
#include "animation/actoranimation.h"

#include "generator/ShaderLoader.h"

#include "utils/ResourceManager.h"

using namespace Idogep;
using namespace Grafkit;

#include "builtin_data/cube.h"

Document::Document() : m_isDirty(false)
{
}

Document::~Document()
{
}

void Document::Preload(IResourceManager * const & resman)
{
	m_vs = resman->Load<ShaderRes>(new VertexShaderLoader("vertexShader", "shaders/vertex.hlsl", ""));
	m_ps = resman->Load<ShaderRes>(new PixelShaderLoader("pixelShader", "shaders/flat.hlsl", ""));
	m_psShowUv = resman->Load<ShaderRes>(new PixelShaderLoader("pixelShader", "shaders/fx/uv.hlsl", ""));

	m_music = resman->Load<MusicRes>(new MusicBassLoader("music/alpha_c_-_euh.ogg"));
}

void Document::Initialize(Renderer & render)
{
	InitTestStuff(render);
}

//void Document::Shutdown()
//{
//}

// ---------------------------------------------------------------
// in-dev-things
// --- TEST STUFF ---
void Document::InitTestStuff(Renderer & render)
{
	// -- model 
	ModelRef model = new Model(new Mesh());
	model->SetMaterial(new Material());
	//model->GetMaterial()->AddTexture(texture, Material::TT_diffuse);
	model->GetMaterial()->SetName("GridMaterial");

	// -- camera
	CameraRef camera = new Camera();
	camera->SetName("Camera");

	ActorRef cameraActor = new Actor(camera);
	cameraActor->SetName("CameraActor");

	cameraActor->Matrix().Identity();
	cameraActor->Matrix().Translate(0, 0, 10);
	m_cameraActor = cameraActor;

	// Add animation here

	model->SetName("cube");
	model->GetMesh()->AddPointer("POSITION", GrafkitData::cubeVertexSize, GrafkitData::cubeVertices);
	model->GetMesh()->AddPointer("TEXCOORD", GrafkitData::cubeVertexSize, GrafkitData::cubeTextureUVs);
	model->GetMesh()->AddPointer("NORMAL", GrafkitData::cubeVertexSize, GrafkitData::cubeNormals);
	model->GetMesh()->SetIndices(GrafkitData::cubeVertexCount, GrafkitData::cubeIndicesCount, GrafkitData::cubeIndices);

	// -- actors

	ActorRef cubeActor = new Actor(model);
	cubeActor->SetName("CubeActor");

	ActorRef rootActor = new Actor();
	rootActor->SetName("RootActor");


	rootActor->AddChild(cameraActor);
	rootActor->AddChild(cubeActor);

	m_rootActor = rootActor;

	// -- scenegraph
	m_scenegraph = new SceneGraph();
	m_scenegraph->SetName("Scene");
	m_scenegraph->SetRootNode(m_rootActor);

	// -- scene 
	m_scene = new SceneRes(new Scene());
	(*m_scene)->SetSceneGraph(m_scenegraph);

	(*m_scene)->AddAnimation(new ActorAnimation(rootActor));
	(*m_scene)->AddAnimation(new ActorAnimation(cameraActor));
	(*m_scene)->AddAnimation(new ActorAnimation(cubeActor));

	(*m_scene)->Initialize();
	(*m_scene)->Build(render, m_vs, m_ps);
}
