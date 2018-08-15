#include <fstream>

#include "schema.h"
#include "demo.h"
#include "json.hpp"

#include "utils/exceptions.h"
#include "utils/logger.h"

#include "utils/asset.h"
#include "utils/AssetFactory.h"
#include "utils/ResourceManager.h"

#include "render/shader.h"
#include "generator/ShaderLoader.h"

#include "render/mesh.h"
#include "render/model.h"
#include "render/material.h"

#include "render/light.h"
#include "render/camera.h"

#include "render/actor.h"
#include "render/SceneGraph.h"

#include "experimental/scene.h"
#include "experimental/AnimationLoader.h"

#include "core/Music.h"
#include "generator/MusicBassLoader.h"

#include "builtin_data/cube.h"
#include "json_fwd.hpp"
#include "animation/actoranimation.h"
#include "json.hpp"
#include "DemoAnimationLoader.h"

using namespace GkDemo;
using namespace Grafkit;
using namespace FWdebugExceptions;

using json = nlohmann::json;

SchemaBuilder::SchemaBuilder() : m_demo(nullptr),m_inited(false) {
}

SchemaBuilder::~SchemaBuilder() {
}

/* ======================================================================
 * Loading stuff
 * ======================================================================  
*/

void SchemaBuilder::LoadFromAsset(const IAssetRef& asset, IResourceManager* resourceManager)
{
    const char* dataPtr = static_cast<const char*>(asset->GetData());
    std::string data(dataPtr, dataPtr + asset->GetSize());

    try
    {
        m_json = Json::parse(data);
        Build(resourceManager, m_json);
    }
    catch (std::exception& e)
    {
        THROW_EX_DETAILS(SchemaParseException, e.what());
    }
}

void SchemaBuilder::Build(IResourceManager*const& resourceManager, const Json& json)
{
    LOGGER(Log::Logger().Info("Loading context"));

    // build 
    BuildResources(resourceManager, json);

    // join includes if any
    Json demoJson = json.at("demo");
    Json demoIncludes = demoJson["includes"];
    if (!demoIncludes.empty())
    {
        for (Json::iterator it = demoIncludes.begin(); it != demoIncludes.end(); ++it)
        {
            const std::string filename = it->get<std::string>();
            const IAssetRef asset = resourceManager->GetAssetFactory()->Get(filename);
            const char* dataPtr = static_cast<const char*>(asset->GetData());
            std::string data(dataPtr, dataPtr + asset->GetSize());
            const Json imported = Json::parse(data);
            BuildResources(resourceManager, imported); // cascade the thing maybe?
        }
    }

    // -- setup demo
    LOGGER(Log::Logger().Info("Building Demo"));
    m_demo = new Demo();

    std::string musicFilename = demoJson.at("music").at("filename").get<std::string>();
    LOGGER(Log::Logger().Info("- Music: %s", musicFilename.c_str()));
    MusicResRef music = resourceManager->Load<MusicRes>(new MusicBassLoader(musicFilename));

    m_demo->SetMusic(music);

    // build scenes

    const Json scenes = demoJson.at("scenes");
    LOGGER(Log::Logger().Info("- Loading Scenes"));
    for (Json::const_iterator scenesIt = scenes.begin(); scenesIt != scenes.end(); ++scenesIt)
    {
        const Json sceneJson = *scenesIt;
        BuildScene(resourceManager, sceneJson);

        const Json animationsJson = sceneJson["animations"];
        if (!animationsJson.empty())
        {
            LOGGER(Log::Logger().Info("- Loading Animations"));
            for (Json::const_iterator animationIt = animationsJson.begin(); animationIt != animationsJson.end(); ++animationIt)
            {
                const Json animationJson = *animationIt;
                BuildAnimation(resourceManager, animationJson);
            }
        }
    }

    const Json animationsJson = demoJson["animations"];
    if (!animationsJson.empty())
    {
        LOGGER(Log::Logger().Info("- Loading Animations"));
        for (Json::const_iterator animationIt = animationsJson.begin(); animationIt != animationsJson.end(); ++animationIt)
        {
            const Json animationJson = *animationIt;
            BuildAnimation(resourceManager, animationJson);
        }
    }


    // buffers? shit?

    // effects ?

    m_inited = false;

}

void SchemaBuilder::BuildResources(IResourceManager*const& resourceManager, const Json& json)
{
    BuildAssets(resourceManager, json.at("assets"));
    BuildSceneGraphs(resourceManager, json.at("scenegraphs"));
}

void SchemaBuilder::BuildObject(const Json& j, const Ref<Object>& ref)
{
    ref->SetName(j.at("name").get<std::string>());
    ref->SetUuid(j.at("uuid").get<std::string>());
    LOGGER(Log::Logger().Trace("Object %s uuid=%s", ref->GetName().c_str(), ref->GetUuid().c_str()));
}

void SchemaBuilder::BuildScene(IResourceManager* const& resourceManager, const Json& sceneJson)
{
    std::string uuid = sceneJson.at("uuid");

    // ids
    uint32_t id = sceneJson.at("id").get<uint32_t>();
    std::string sceneUuid = sceneJson.at("scene");

    LOGGER(Log::Logger().Info("-- Scene id=%d uuid=%s", id, uuid.c_str()));

    Ref<Resource<SceneGraph>> sceneResource = resourceManager->GetByUuid<Resource<SceneGraph>>(sceneUuid);
    if (sceneResource.Valid() , sceneResource->Valid())
    {
        SceneGraphRef sceneGraph = sceneResource->Get();

        LOGGER(Log::Logger().Info("--- SceneGraph %s uuid=%s",
                sceneResource->GetName().c_str(),
            sceneResource->GetUuid().c_str()
            ));

        // TODO add scene here 
        SceneRef scene = new Scene();
        BuildObject(sceneJson, scene);
        scene->SetSceneGraph(sceneResource);
        // It will build itself later on

        resourceManager->Add(new SceneRes(scene, scene->GetName(), scene->GetUuid()));
    }
    else
    {
        // TODO: throw
        THROW_EX_DETAILS(SchemaParseException, "Nincs Scene");
    }
}

void SchemaBuilder::BuildAnimation(IResourceManager* const & resourceManager, const Json& animationJson)
{
    //Ref<Resource<Animation>> animation;

    std::string type = animationJson.at("type");
    std::string name = animationJson.at("name");
    std::string uuid = animationJson.at("uuid");
    std::string targetUuid = animationJson.at("target");
    LOGGER(Log::Logger().Info("-- Animation %s %s uuid=%s target=%s ", type.c_str(), name.c_str(), uuid.c_str(), targetUuid.c_str()));
    if (type.compare("actor") == 0) { resourceManager->Load<Resource<Animation>>(new ActorAnimationLoader(name, "animations/" + uuid, uuid, targetUuid)); }
    if (type.compare("demo") == 0) { resourceManager->Load<Resource<Animation>>(new DemoAnimationLoader(name, "animations/" + uuid, uuid, targetUuid)); }
}

void SchemaBuilder::BuildMesh(IResourceManager*const& resourceManager, const Json& json)
{
    MeshRef mesh;
    std::string generator = json.at("generator");

    LOGGER(Log::Logger().Info("--- Invoke generator %s", generator.c_str()));
    if (generator.compare("cube") == 0) { mesh = GrafkitData::CreateCube(); }
    //else if (generator.compare("cube") == 0)
    //{
    //}

    if (!mesh)
    THROW_EX_DETAILS(SchemaParseException, "Could not add mesh");
    BuildObject(json, mesh);
    resourceManager->Add(new Resource<Mesh>(mesh, mesh->GetName(), mesh->GetUuid()));
}

void SchemaBuilder::BuildMaterial(IResourceManager* const& resourceManager, const Json& it)
{
    MaterialRef material = new Material();
    BuildObject(it, material);
    resourceManager->Add(new Resource<Material>(material, material->GetName(), material->GetUuid()));

    // TODO: ... 
}

void SchemaBuilder::BuildShader(IResourceManager*const& resourceManager, const Json shader)
{
    ShaderResRef shaderRes;
    std::string name = shader.at("name").get<std::string>();
    std::string uuid = shader.at("uuid").get<std::string>();
    std::string type = shader.at("type").get<std::string>();
    std::string filename = shader.at("filename").get<std::string>();

    std::string entrypoint = shader["entrypoint"].get<std::string>();

    if (type.compare("ps") == 0)
    {
        LOGGER(Log::Logger().Info("--- PixelShader %s uuid=%s", name.c_str(), uuid.c_str()));
        shaderRes = resourceManager->Load<ShaderRes>(new PixelShaderLoader(name, filename, entrypoint, uuid));
    }
    else if (type.compare("vs") == 0)
    {
        LOGGER(Log::Logger().Info("--- VertexSahder %s uuid=%s", name.c_str(), uuid.c_str()));
        shaderRes = resourceManager->Load<ShaderRes>(new VertexShaderLoader(name, filename, entrypoint, uuid));
    }

    shaderRes->SetName(name);
}

void SchemaBuilder::BuildAssets(IResourceManager*const& resourceManager, const Json& json)
{
    LOGGER(Log::Logger().Info("- Loading json"));

    // --- 
    const Json meshJson = json["meshes"];
    if (!meshJson.empty())
    {
        LOGGER(Log::Logger().Info("-- Loading meshes"));
        for (Json::const_iterator meshIt = meshJson.begin(); meshIt != meshJson.end(); ++meshIt) { BuildMesh(resourceManager, *meshIt); }
    }

    // --- 
    const Json camerasJson = json["cameras"];
    if (!camerasJson.empty())
    {
        LOGGER(Log::Logger().Info("-- Loading camseras"));
        for (Json::const_iterator cameraIt = camerasJson.begin(); cameraIt != camerasJson.end(); ++cameraIt)
        {
            CameraRef camera = new Camera();
            BuildObject(*cameraIt, camera);
            resourceManager->Add(new Resource<Camera>(camera, camera->GetName(), camera->GetUuid()));
        }
    }

    // ---
    const Json lightsJson = json["lights"];
    if (!lightsJson.empty())
    {
        LOGGER(Log::Logger().Info("-- Loading lights"));
        for (Json::const_iterator lightIt = lightsJson.begin(); lightIt != lightsJson.end(); ++lightIt)
        {
            LightRef light = new Light();
            BuildObject(*lightIt, light);
            resourceManager->Add(new Resource<Light>(light, light->GetName(), light->GetUuid()));
        }
    }

    // --- 
    const Json models = json["models"];
    if (!models.empty())
    {
        LOGGER(Log::Logger().Info("-- Loading models"));
        for (Json::const_iterator modelIt = models.begin(); modelIt != models.end(); ++modelIt)
        {
            ModelRef model = new Model();
            BuildObject(*modelIt, model);
            resourceManager->Add(new Resource<Model>(model, model->GetName(), model->GetUuid()));
        }
    }

    // --- 
    const Json materials = json["materials"];
    if (!materials.empty())
    {
        LOGGER(Log::Logger().Info("-- Loading materials"));
        for (Json::const_iterator materialIt = materials.begin(); materialIt != materials.end(); ++materialIt) { BuildMaterial(resourceManager, *materialIt); }
    }

    // --- 
    const Json shaders = json["shaders"];
    if (!shaders.empty())
    {
        LOGGER(Log::Logger().Info("-- Loading shaders"));
        for (Json::const_iterator shaderIt = shaders.begin(); shaderIt != shaders.end(); ++shaderIt) { BuildShader(resourceManager, *shaderIt); }
    }

}

void SchemaBuilder::BuildSceneGraphs(IResourceManager*const& resourceManager, const Json& json)
{
    LOGGER(Log::Logger().Info("- Loading scenegraphs"));
    if (!json.empty())
    {
        for (Json::const_iterator scenegraphIt = json.begin(); scenegraphIt != json.end(); ++scenegraphIt)
        {
            SceneGraphRef scenegraph = new SceneGraph();
            BuildObject(*scenegraphIt, scenegraph);

            resourceManager->Add(new Resource<SceneGraph>(scenegraph, scenegraph->GetName(), scenegraph->GetUuid()));

            const Json actors = scenegraphIt->at("actors");
            if (!actors.empty())
            {
                // create
                LOGGER(Log::Logger().Info("-- Loading actors"));
                for (Json::const_iterator actorIt = actors.begin(); actorIt != actors.end(); ++actorIt)
                {
                    ActorRef actor = new Actor();
                    BuildObject(*actorIt, actor);
                    resourceManager->Add(new Resource<Actor>(actor, actor->GetName(), actor->GetUuid()));
                }

                //assign
                LOGGER(Log::Logger().Info("-- Assign actors"));
                for (Json::const_iterator actorIt = actors.begin(); actorIt != actors.end(); ++actorIt)
                {
                    Ref<Resource<Actor>> actorRes = resourceManager->GetByUuid<Resource<Actor>>(actorIt->at("uuid").get<std::string>());
                    if (actorRes.Valid() && actorRes->Valid())
                    {
                        ActorRef actorRef = static_cast<Ref<Actor>>(*actorRes);
                        const Json actorJson = *actorIt;
                        ExtractActorParent(resourceManager, actorJson, actorRef);
                        ExtractEntities(resourceManager, actorJson, actorRef);
                    }
                }
            }

            Ref<Resource<Actor>> rootActorRes = resourceManager->GetByUuid<Resource<Actor>>(scenegraphIt->at("rootActor").get<std::string>());
            if (rootActorRes.Valid() && rootActorRes->Valid())
            {
                LOGGER(Log::Logger().Info("-- Root actor %s uuid=%s", rootActorRes->GetName().c_str(), rootActorRes->GetUuid().c_str()));
                scenegraph->SetRootNode(*rootActorRes);
            }
            else { // Todo Throw EX
            }
        }
    }
}

void SchemaBuilder::BuildEffects(IResourceManager*const& resourceManager, const Json& effects) {
}

/*
 * Wiring stuff together
 */
void SchemaBuilder::Initialize(IResourceManager*const& resourceManager) { 
    if (m_json.empty() || m_json.is_discarded())
        return;

    if (m_inited)
        return;

    LOGGER(Log::Logger().Info("Initializing"));


    const Json assetsJson = m_json.at("assets");
    AssignAssets(resourceManager, assetsJson);

    const Json demoJson = m_json.at("demo");

    const Json scenesJson = demoJson.at("scenes");

    LOGGER(Log::Logger().Info("- Loading Scenes"));
    for (Json::const_iterator scenesIt = scenesJson.begin(); scenesIt != scenesJson.end(); ++scenesIt)
    {
        const Json sceneJson = *scenesIt;
        std::string uuid = sceneJson.at("uuid");
        uint32_t id = sceneJson.at("id").get<uint32_t>();

        SceneResRef scene = resourceManager->GetByUuid<SceneRes>(uuid);

        if (scene && scene->Valid())
        {
            m_demo->AddScene(id, scene);
            AssignAnimations(resourceManager,sceneJson);
        }

        AssignAnimations(resourceManager,demoJson);
    }

    AssignShader(resourceManager, demoJson.at("render").at("forward"));

    m_inited = true;
}

void SchemaBuilder::AssignAnimations(IResourceManager* const& resourceManager, const Json & json)
{
    const Json animationsJson = json.at("animations");

    std::string uuid = json.at("uuid").get<std::string>();

    SceneResRef scene = resourceManager->GetByUuid<SceneRes>(uuid);

    for (Json::const_iterator animationIt = animationsJson.begin(); animationIt != animationsJson.end(); ++animationIt)
    {
        const Json animationJson = *animationIt;
        std::string type = animationJson.at("type");
        std::string name = animationJson.at("name");
        std::string uuid = animationJson.at("uuid");
        std::string targetUuid = animationJson.at("target");
        if (type.compare("actor") == 0)
        {
            ActorAnimationResRef animation = resourceManager->GetByUuid<ActorAnimationRes>(uuid);
            Ref<Resource<Actor>> actor = resourceManager->GetByUuid<Resource<Actor>>(targetUuid);
            if (animation && actor && *animation && *actor)
            {
                assert(*scene);
                (*animation)->SetActor(static_cast<Ref<Actor>>(*actor));
                (*scene)->AddAnimation(*animation);
            } else
            {
                assert(0);
            }
        }
        if (type.compare("demo") == 0)
        {
            Ref<Resource<DemoAnimation>> animation = resourceManager->GetByUuid<Resource<DemoAnimation>>(uuid);
            m_demo->AddAnimation(*animation);
        }
    }
}

void SchemaBuilder::AssignShader(IResourceManager* const& resourceManager, Json sceneJson)
{
    Ref<Resource<SceneGraph>> sceneResource;
    std::string vsUuid = sceneJson.at("vs").get<std::string>();
    std::string psUuid = sceneJson.at("ps").get<std::string>();

    ShaderResRef ps = resourceManager->GetByUuid<ShaderRes>(psUuid);
    ShaderResRef vs = resourceManager->GetByUuid<ShaderRes>(vsUuid);

    if (ps && vs) // threst will be preloaded
    {
        LOGGER(Log::Logger().Info("--- PS %s uuid=%s", ps->GetName().c_str(), ps->GetUuid().c_str()));
        m_demo->SetPs(ps);

        LOGGER(Log::Logger().Info("--- VS %s uuid=%s ", vs->GetName().c_str(), vs->GetUuid().c_str()));
        m_demo->SetVs(vs);
    }
    else
    {
        // todo: throw
        THROW_EX_DETAILS(SchemaParseException, "Nincs PS vagy VS");
    }
}

Ref<Demo> SchemaBuilder::GetDemo() const { return m_demo; }


void SchemaBuilder::AssignModel(IResourceManager*const& resourceManager, const Json& json)
{
    Ref<Resource<Model>> model = resourceManager->GetByUuid<Resource<Model>>(json.at("uuid").get<std::string>());

    if (model.Valid() && model->Valid())
    {
        LOGGER(Log::Logger().Info("--- Model %s uuid=%s", (*model)->GetName().c_str(), model->GetUuid().c_str()));

        Ref<Resource<Mesh>> mesh = resourceManager->GetByUuid<Resource<Mesh>>(json.at("mesh").get<std::string>());
        if (mesh.Valid() && mesh->Valid())
        {
            LOGGER(Log::Logger().Info("---- Mesh %s uuid=%s", (*mesh)->GetName().c_str(), mesh->GetUuid().c_str()));
            (*model)->SetMesh(static_cast<Ref<Mesh>>(*mesh));
        }

        Ref<Resource<Material>> material = resourceManager->GetByUuid<Resource<Material>>(json.at("material").get<std::string>());
        if (material.Valid() && material->Valid())
        {
            LOGGER(Log::Logger().Info("---- Material %s uuid=%s", (*material)->GetName().c_str(), material->GetUuid().c_str()));
            (*model)->SetMaterial(static_cast<Ref<Material>>(*material));
        }
    }
}

void SchemaBuilder::AssignAssets(IResourceManager*const& resourceManager, const Json& assets)
{
    const Json models = assets["models"];
    if (!models.empty())
    {
        LOGGER(Log::Logger().Info("-- Assign models"));
        for (Json::const_iterator it = models.begin(); it != models.end(); ++it) { AssignModel(resourceManager, *it); }
    }
}

void SchemaBuilder::ExtractActorParent(IResourceManager*const& resourceManager, const Json& json, const ActorRef& childActorRef)
{
    const Json parentJson = json["parent"];
    if (!parentJson.empty() && !parentJson.is_null())
    {
        Ref<Resource<Actor>> parentRes = resourceManager->GetByUuid<Resource<Actor>>(parentJson.get<std::string>());
        if (parentRes.Valid() && parentRes->Valid())
        {
            LOGGER(Log::Logger().Info("--- Parent %s uuid=%s child: %s uuid=%s",
                    parentRes->GetName().c_str(),
                parentRes->GetUuid().c_str(),
                childActorRef->GetName().c_str(),
                childActorRef->GetUuid().c_str()
                ));

            (*parentRes)->AddChild(childActorRef);
        }
        else
        {
            LOGGER(Log::Logger().Warn("--- No Parent found %s", parentJson.get<std::string>().c_str()));
        }
    }
}

void SchemaBuilder::ExtractEntities(IResourceManager*const& resourceManager, const Json& json, ActorRef& actorRef)
{
    const Json entities = json["entities"];
    if (!entities.empty())
    {
        for (Json::const_iterator entityIt = entities.begin(); entityIt != entities.end(); ++entityIt)
        {
            std::string uuid = entityIt->get<std::string>();
            // terrible hack
            Ref<IResource> entityRes = resourceManager->GetByUuid<IResource>(uuid);
            if (entityRes.Valid())
            {
                Ref<Entity3D> entityRef = dynamic_cast<Entity3D*>(reinterpret_cast<Object*>(entityRes->GetRaw()));
                if (entityRef.Valid())
                {
                    LOGGER(Log::Logger().Info("--- Entity"));
                    actorRef->AddEntity(entityRef);
                }
            }
        }
    }
}
