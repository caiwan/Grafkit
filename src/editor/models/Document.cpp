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
#include "generator/MusicBassLoader.h"

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

SceneGraphRef Document::GetScenegraph() const
{
	return m_scenegraph;
}

SceneResRef Document::GetScene() const
{
	return m_scene;
}

MusicResRef Document::GetMusic() const
{
	return m_music;
}

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

	for (size_t i = 0; i < (*m_scene)->GetAnimationCount(); ++i)
	{
		AnimationRef animation = (*m_scene)->GetAnimation(i);

		for (size_t j = 0; j < animation->GetTrackCount(); ++j)
		{
			Ref<Animation::Track> track = animation->GetTrack(j);

			for (size_t k = 0; k < track->GetChannelCount(); ++k)
			{
				Ref<Animation::Channel> channel = track->GetChannel(k);
				for (size_t l = 0; l < 256; ++l)
				{
					const float v = 2 * static_cast<float>(rand()) / static_cast<float>(RAND_MAX) - 1;
				    const float t = 1. + l;

					Animation::Key key(t, v);
					//key.m_type = Animation::KI_step;
					key.m_type = static_cast<Animation::KeyInterpolation_e>(l % Animation::KI_COUNT);
					channel->AddKey(key);

				    //Animation::Key key2(t + .5, 0.);
					//key2.m_type = Animation::KI_step;
					//channel->AddKey(key2);
				}
			}
		}

	}

	(*m_scene)->Initialize();
	(*m_scene)->Build(render, m_vs, m_ps);
}
