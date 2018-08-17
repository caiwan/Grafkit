#include "context.h"

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

using namespace Grafkit;
using namespace GkDemo;
using namespace GrafkitData;


Context::Context(Renderer& render, IAssetFactory* assetFactory) : ResourcePreloader()
    , ClonableInitializer()
    , m_render(render)
    , m_demo(nullptr)
    , m_assetFactory(assetFactory) {
}

Context::~Context() = default;

void Context::Relocate(std::string path)
{
    if (!m_demo)
        return;
 
    LOGGER(Log::Logger().Info("Relocate context path %s", path.c_str()));

    m_assetFactory->SetBasePath(path);
    delete m_demo;
    m_demo = nullptr;
    this->ClearLoadStack();
    this->RemoveAll();

    // LOGGER(Log::Logger().Info("Relocate context path %s", path.c_str()));
}

void Context::SaveSchema() {
}

void Context::LoadScema() {
    m_builder.LoadFromAsset(m_assetFactory->Get("/schema.json"), this);
    m_demo = m_builder.GetDemo();
    assert(m_demo);
}

void Context::Intitialize()
{
    IResourceManager *const & resman = this;
    m_builder.Initialize(resman); m_demo->Initialize(m_render);
}

void Context::CreateTestStuff()
{
    Demo* demo = new Demo();

    ShaderResRef vs = Load<ShaderRes>(new VertexShaderLoader("vertexShader", "shaders/vertex.hlsl", ""));
    ShaderResRef ps = Load<ShaderRes>(new PixelShaderLoader("pixelShader", "shaders/flat.hlsl", ""));

    MusicResRef music = Load<MusicRes>(new MusicBassLoader("music/alpha_c_-_euh.ogg"));

    demo->SetMusic(music);
    demo->SetPs(ps);
    demo->SetVs(vs);

    demo->AddAnimation(new DemoAnimation());
    demo->GetAnimation(0)->Initialize();

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

    cameraAnimation->GetTrack(0)->SetFloat3(0, float3(0, 0, 10));

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

    demo->AddScene(0, scene);

    m_demo = demo;
}
