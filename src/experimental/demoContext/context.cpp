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
#include "scene/scene.h"
#include "animation/actoranimation.h"

#include "resource/loaders/ShaderLoader.h"

#include "resource/ResourceManager.h"

#include "utils/asset/AssetFactory.h"
#include "core/Asset.h"

#include "core/Music.h"
#include "resource/loaders/MusicBassLoader.h"

#include "schema.h"

#include "builtin_data/cube.h"
#include "utils/persistence/archive.h"

using namespace Grafkit;
using namespace GkDemo;
using namespace GrafkitData;

#define ANIMATION_ROOT "animation/"

#if 0
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

    m_myBasePath = path;

    m_assetFactory->SetBasePath(path);
    //delete m_demo;
    m_demo = nullptr;
    this->ClearLoadStack();
    this->RemoveAll();
}

void Context::SaveSchema(bool isAutoSave) const
{
    size_t animationCount = m_demo->GetAnimationCount();
    AnimationRef animation;
    for (size_t i = 0; i < animationCount; i++)
    {
        animation = m_demo->GetAnimation(i);
        SaveObject(animation, ANIMATION_ROOT, isAutoSave);
    }
    size_t sceneCount = m_demo->GetSceneCount();
    for (size_t i = 0; i < sceneCount; i++)
    {
        SceneResRef scene = m_demo->GetScene(i);
        if (!scene || !*scene)
            continue;
        size_t sceneAnimationCount = (*scene)->GetAnimationCount();
        for (size_t j = 0; j < sceneAnimationCount; j++)
        {
            animation = (*scene)->GetAnimation(j);
            SaveObject(animation, ANIMATION_ROOT, isAutoSave);
        }

        // models + meshes?
    }

    // we have to purge the FS watchers state due to prevent trigger reload again
    // Todo: find a better way to purge stuff 
    do {} while(m_assetFactory->PollEvents(nullptr));
}

void Context::LoadScema()
{
    m_builder.LoadFromAsset(m_assetFactory->Get("/schema.json"), this);
    m_demo = m_builder.GetDemo();
    assert(m_demo);
}

void Context::Intitialize()
{
    LOGGER(Log::Logger().Debug("Demo ptr: %p", m_demo.Get()));
    IResourceManager*const & resman = this;
    m_builder.Initialize(resman);
    m_demo->SetIsFxaa(m_isFxaa);
    m_demo->Initialize(m_render);
}

Ref<Demo> Context::GetDemo() const { return m_demo; }

void Context::SetDemo(const Ref<Demo>& demo) { m_demo = demo; }

void Context::CreateTestStuff()
{
    Ref<Demo> demo = new Demo();

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

void Context::SaveObject(const Ref<Object>& ref, const char* path, bool isAutoSave) const {
    FILE * fp = nullptr;

    std::string savepath = m_myBasePath + "/" + path + ref->GetUuid();
    if (isAutoSave)
        savepath = savepath + ".autosave";

    errno_t err = fopen_s(&fp, savepath.c_str(), "wb");

    if (0 == err)
    {
        ArchiveFile archive(fp, true);
        ref->Store(archive);
        fflush(fp);
        fclose(fp);
    } else
    {
        assert(0);
    }

}

#endif 