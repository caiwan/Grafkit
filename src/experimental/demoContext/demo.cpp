#include "demo.h"

#include "math/fbm.h"

#include "render/shader.h"
#include "render/effect.h"

#include "render/mesh.h"
#include "render/model.h"
#include "render/material.h"
#include "render/texture.h"

#include "render/SceneGraph.h"
#include "experimental/scene.h"
#include "animation/actoranimation.h"

#include "generator/ShaderLoader.h"

#include "utils/ResourceManager.h"
#include "generator/MusicBassLoader.h"

#include "core/Music.h"

#include "schema.h"

#include "builtin_data/cube.h"

using namespace GkDemo;
using namespace Grafkit;
using namespace GrafkitData;

// ============================================================================================

Demo::Demo(): m_activeSceneId(0) {
}

Demo::~Demo() {
}

void Demo::Preload(IResourceManager* const& resman)
{
    m_vs = resman->Load<ShaderRes>(new VertexShaderLoader("vertexShader", "shaders/vertex.hlsl", ""));
    m_ps = resman->Load<ShaderRes>(new PixelShaderLoader("pixelShader", "shaders/flat.hlsl", ""));

    m_music = resman->Load<MusicRes>(new MusicBassLoader("music/alpha_c_-_euh.ogg"));
}

void Demo::Initialize(Renderer& render)
{
    AddAnimation(new DemoAnimation());
    InitTestStuff(render);

    for (size_t i = 0; i < GetAnimationCount(); ++i)
    {
        AnimationRef animation = GetAnimation(i);
        animation->Initialize();
    }
}

SceneResRef Demo::GetActiveScene() const {
    return m_scenes.at(m_activeSceneId);
}

int Demo::PreRender(Renderer& render, float time) const {
    return 0;
}


int Demo::Render(Renderer& render, float time) const
{
    // --- 
    render.BeginScene();

    float &t = time;

    SceneResRef scene = GetActiveScene();
    if (scene)
    {
        (*scene)->RenderFrame(render, t);
    }

    render.EndScene();

    // ---
    return 0;
}

ShaderResRef Demo::GetPs() const { return m_ps; }

void Demo::SetPs(const ShaderResRef& resource) { m_ps = resource; }

MusicResRef Demo::GetMusic() const { return m_music; }

void Demo::AddScene(uint32_t id, const SceneResRef& ref) {
    if (id >= m_scenes.size())
    {
        fill_n(back_inserter(m_scenes), id - m_scenes.size() + 1, nullptr);
    }

    m_scenes[id] = ref;
}

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

    // -- scenegraph
    SceneGraphRef sceneGraph = new SceneGraph();
    sceneGraph->SetName("Scene");
    sceneGraph->SetRootNode(rootActor);

    // -- scene 
    SceneResRef scene = new SceneRes(new Scene());
    (*scene)->SetSceneGraph(sceneGraph);
    (*scene)->AddAnimation(new ActorAnimation(rootActor));
    (*scene)->AddAnimation(new ActorAnimation(cubeActor));

    AnimationRef cameraAnimation = new ActorAnimation(cameraActor);
    (*scene)->AddAnimation(cameraAnimation);

    for (size_t i = 0; i < (*scene)->GetAnimationCount(); ++i)
    {
        AnimationRef animation = (*scene)->GetAnimation(i);
        animation->Initialize();
    }

    cameraAnimation->GetTrack(0)->SetFloat3(0, float3(0,0,10));

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

    (*scene)->Initialize();
    (*scene)->Build(render, m_vs, m_ps);

    m_scenes.push_back(scene);
}

ShaderResRef Demo::GetVs() const { return m_vs; }

void Demo::SetVs(const ShaderResRef& resource) { m_vs = resource; }

// ============================================================================================

PERSISTENT_IMPL(GkDemo::DemoAnimation);

void DemoAnimation::Initialize()
{
    m_activeScene = new Track("ActiveScene");
    m_activeScene->CreateChannel("id");
    AddTrack(m_activeScene);
}

void DemoAnimation::Update(double time) {
    if (m_target)
    {
        float id = floor(m_activeScene->GetChannel(0)->GetValue(time));
        m_target->SetActiveSceneId(static_cast<uint32_t>(id));
        // ... 
    }
}

void DemoAnimation::Serialize(Archive & ar)
{
}
