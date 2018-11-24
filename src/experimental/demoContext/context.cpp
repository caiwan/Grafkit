#include "stdafx.h"

#include <queue>
#include <locale>

#include "context.h"

#include "json.hpp"

#include "demo.h"
#include "core/Asset.h"

#include "utils/persistence/archive.h"

//#include "render/shader.h"
//#include "render/effect.h"
#include "render/mesh.h"
//#include "render/model.h"
//#include "render/material.h"
#include "render/texture.h"
//#include "render/SceneGraph.h"

#include "scene/scene.h"
#include "animation/actoranimation.h"

#include "resource/ResourceManager.h"

#include "utils/asset/AssetFactory.h"
#include "core/Music.h"

#include "builtin_data/cube.h"

#include "loaders/MeshLoader.h"
#include "resource/loaders/TextureLoader.h"
#include "resource/loaders/ShaderLoader.h"
#include "resource/loaders/MusicBassLoader.h"

using namespace Grafkit;
using namespace GkDemo;
using namespace GrafkitData;

#define ANIMATION_ROOT "animation/"

// TODO : separate file(s)
namespace GkDemo
{
    // -- Model
    void from_json(const Json& json, MeshLoadParams& params)
    {
        params.filename = json.at("filename").get<std::string>();
        params.typeHint = json.at("type_hint").get<std::string>();
    }
}

namespace Grafkit
{
    // -- Texture

    void from_json(const Json& json, TextureBitmapParams& params) { params.filename = json.at("filename").get<std::string>(); }

    void from_json(const Json& json, TextureCubemapParams& params)
    {
        const auto f = json.at("filenames");
        std::transform(f.begin(), f.end(), params.sourceNames.begin(), [](const Json& json) { return json.get<std::string>(); });
    }

    void from_json(const Json& json, TextureNoiseParams& params) { params.size = json.at("size").get<uint16_t>(); }

    void from_json(const Json& json, TextureSamplerParams& params) { params.mode = json.at("mode").get<TextureSamplerParams::AddressMode>(); }
}


Context::Context(Renderer& render, const std::shared_ptr<IAssetFactory>& assetFactory) : m_isFxaa(false)
    , m_render(render)
    , m_assetFactory(assetFactory), m_demo(nullptr)
{
#define P_LIST(json) json.at("name").get<std::string>(), json.at("uuid").get<std::string>(), json.at("params")

    //m_loaders["demo"] = [](const Json& json) { return new DemoLoader(P_LIST(json)); };
    m_loaders["mesh"] = [](const Json& json) { return new MeshLoader(P_LIST(json)); };
    m_loaders["texture_2d"] = [](const Json& json) { return new TextureFromBitmap(P_LIST(json)); };
    m_loaders["texture_cube"] = [](const Json& json) { return new TextureCubemapFromBitmap(P_LIST(json)); };
    m_loaders["texture_noise"] = [](const Json& json) { return new TextureNoiseMapBuilder(P_LIST(json)); };
    m_loaders["texture_sampler"] = [](const Json& json) { return new TextureSamplerBuilder(P_LIST(json)); };
    // ... 

#undef P_LIST
}


Context::~Context() {
}

Json Context::LoadJson(const StreamRef& schemaAsset)
{
    Json rootJson = Json::parse(static_cast<std::istream&>(*schemaAsset));
    // flatten all the included stuff
    const auto includeIt = rootJson.find("include");
    if (includeIt != rootJson.end() && includeIt.value().is_array() && !includeIt.value().empty())
    {
        std::stack<std::string> stack;
        std::for_each(includeIt.value().begin(), includeIt.value().end(), [&stack](const auto& elem) { stack.push(elem.get<std::string>()); });
        while (!stack.empty())
        {
            // pop
            std::string filename = stack.top();
            stack.pop();

            // load included stuff
            try
            {
                auto jsonAsset = m_assetFactory->Get(filename);
                Json loadJson = Json::parse(static_cast<std::istream&>(*jsonAsset));

                // todo: ... fk

                // push back
                const auto loadedIncludeIt = loadJson.find("include");
                if (loadedIncludeIt != loadJson.end())
                    std::for_each(includeIt.value().begin(), includeIt.value().end(), [&stack](const auto& elem) { stack.push(elem.get<std::string>()); });
            }
            catch (std::exception& e)
            {
                LOGGER(Log::Logger().Warn("Not found included json %s", filename.c_str()));
            }
        }
    }

    return rootJson;
}

void Context::LoadDemo(const std::string filename)
{
    StreamRef schemaAsset = m_assetFactory->Get(filename);
    const Json demoJson = LoadJson(schemaAsset);

    // -- asset
    try
    {
        const Json assetJson = demoJson.at("assets");
        for (auto& assetElem : assetJson)
        {
            std::string generatorType = assetElem.at("generator");
            transform(generatorType.begin(), generatorType.end(), generatorType.begin(), tolower);
            auto loaderIt = m_loaders.find(generatorType);
            if (loaderIt != m_loaders.end()) { this->Load(loaderIt->second(assetElem)); }
        }
    }
    catch (std::exception& e) { throw std::runtime_error(std::string("Cannot load assets (") + e.what() + std::string(")")); }

    // -- demo
    m_demo = GetByUuid<Resource<Demo>>(demoJson['demo'].get<std::string>());
    //if (!m_demo)
    //    throw std::runtime_error("No demo, you bastard! Havent't you forget something?");
}


// TODO -> Editor
void Context::Relocate(std::string path)
{
    assert(0);
}

// TODO -> Editor
void Context::SaveSchema(bool isAutoSave) const
{
    assert(0);
}

Ref<Resource<Demo>> Context::GetDemo() const { return m_demo; }

void Context::SetDemo(const Ref<Resource<Demo>>& demo) { m_demo = demo; }

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
    do {} while (m_assetFactory->PollEvents(nullptr));
}

void Context::Load()
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
    }
    else
    {
        assert(0);
    }

}

#endif
