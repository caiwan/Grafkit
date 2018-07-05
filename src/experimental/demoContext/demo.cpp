#include "demo.h"

#include "math/fbm.h"

#include "render/shader.h"
#include "render/effect.h"

#include "render/mesh.h"
#include "render/model.h"
#include "render/material.h"

#include "render/SceneGraph.h"
#include "animation/scene.h"
#include "animation/actoranimation.h"

#include "generator/ShaderLoader.h"

#include "utils/ResourceManager.h"
#include "generator/MusicBassLoader.h"

#include "builtin_data/cube.h"

using namespace GkDemo;
using namespace Grafkit;
using namespace GrafkitData;

Demo::Demo() {
}

Demo::~Demo() {
}

void Demo::Preload(IResourceManager* const& resman)
{
    m_vs = resman->Load<ShaderRes>(new VertexShaderLoader("vertexShader", "shaders/vertex.hlsl", ""));
    m_ps = resman->Load<ShaderRes>(new PixelShaderLoader("pixelShader", "shaders/flat.hlsl", ""));
    m_psShowUv = resman->Load<ShaderRes>(new PixelShaderLoader("pixelShader", "shaders/fx/uv.hlsl", ""));

    m_music = resman->Load<MusicRes>(new MusicBassLoader("music/alpha_c_-_euh.ogg"));
}

void Demo::Initialize(Renderer& render) { InitTestStuff(render); }

int Demo::PreRender(Renderer& render, float time) const {
    return 0;
}

int Demo::Render(Renderer& render, float time) const
{
    // --- 

    render.BeginScene();

    float &t = time;

    this->m_rootActor->Matrix().Identity();
    //this->m_rootActor->Matrix().RotateRPY(0, 0, 0);
    this->m_rootActor->Matrix().Translate(0, 0, 0);

    this->m_cameraActor->Matrix().Identity();
    this->m_cameraActor->Matrix().RotateRPY(M_PI / 4., 0, M_PI / 4.);
    this->m_cameraActor->Matrix().Translate(0, 0, 10);

    (*m_scene)->RenderFrame(render, t);

    render.EndScene();

    // ---
    return 0;
}

SceneGraphRef Demo::GetScenegraph() const { return m_scenegraph; }

SceneResRef Demo::GetScene() const { return m_scene; }

MusicResRef Demo::GetMusic() const { return m_music; }

// This thing serves testing purposes for the editor.
void Demo::InitTestStuff(Renderer& render) {
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
    model->GetMesh()->AddPointer("POSITION", cubeVertexSize, cubeVertices);
    model->GetMesh()->AddPointer("TEXCOORD", cubeVertexSize, cubeTextureUVs);
    model->GetMesh()->AddPointer("NORMAL", cubeVertexSize, cubeNormals);
    model->GetMesh()->SetIndices(cubeVertexCount, cubeIndicesCount, cubeIndices);

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

#if 0
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
                    key.m_type = Animation::KI_hermite;
                    //key.m_type = static_cast<Animation::KeyInterpolation_e>(l % Animation::KI_COUNT);
                    channel->AddKey(key);

                    //Animation::Key key2(t + .5, 0.);
                    //key2.m_type = Animation::KI_step;
                    //channel->AddKey(key2);
                }
            }
        }

    }

#endif

    (*m_scene)->Initialize();
    (*m_scene)->Build(render, m_vs, m_ps);
}
