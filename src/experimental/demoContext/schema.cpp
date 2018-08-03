#include <fstream>

#include "schema.h"
#include "demo.h"
#include "json.hpp"

#include "render/shader.h"

#include "render/mesh.h"
#include "render/model.h"
#include "render/material.h"

#include "render/actor.h"
#include "render/SceneGraph.h"

#include "utils/asset.h"
#include "utils/ResourceManager.h"

#include "utils/exceptions.h"
#include "utils/logger.h"

#include "core/Music.h"
#include "generator/MusicBassLoader.h"

#include "builtin_data/cube.h"
#include "utils/AssetFactory.h"
#include "json.hpp"

using namespace GkDemo;
using namespace Grafkit;
using namespace FWdebugExceptions;

using json = nlohmann::json;

SchemaBuilder::SchemaBuilder()
{
}

SchemaBuilder::~SchemaBuilder()
{
}


void SchemaBuilder::LoadFromAsset(const IAssetRef& asset, IResourceManager* resourceManager) {
    const char * dataPtr = static_cast<const char*>(asset->GetData());
    std::string data(dataPtr, dataPtr + asset->GetSize());

    try {
        const Json json = Json::parse(data);
        Build(resourceManager, json);
    }
    catch (std::exception &e) {
        THROW_EX_DETAILS(SchemaParseException, e.what());
    }

}

void SchemaBuilder::Initialize() const {
}

Ref<Demo> SchemaBuilder::GetDemo() const { return m_demo; }


void SchemaBuilder::BuildResources(IResourceManager*const& resourceManager, const Json& json) {
    BuildAssets(resourceManager, json.at("json"));
    BuildSceneGraphs(resourceManager, json.at("scenegraphs"));
}

void SchemaBuilder::Build(IResourceManager*const& resourceManager, const Json& json) {
    LOGGER(Log::Logger().Info("Loading context"));

    // build 
    BuildResources(resourceManager, json);

    // join includes if any
    Json demo = json.at("demo");
    Json demoIncludes = demo["includes"];
    if (!demoIncludes.empty())
    {
        for (Json::iterator it= demoIncludes.begin(); it!=demoIncludes.end(); ++it)
        {
            const std::string filename = it->get<std::string>();
            const IAssetRef asset = resourceManager->GetAssetFactory()->Get(filename);
            const char * dataPtr = static_cast<const char*>(asset->GetData());
            std::string data(dataPtr, dataPtr + asset->GetSize());
            const Json imported = Json::parse(data);
            BuildResources(resourceManager, imported); // cascade maybe?
        }
    }

    m_demo = new Demo();
    MusicResRef music = resourceManager->Load<MusicRes>(new MusicBassLoader(demo.at("music").get<std::string>()));
    //m_demo->SetPs()
    //m_demo->SetVs()

    // buffers? shit?

    // effects ?
}

void SchemaBuilder::BuildObject(const Json& j, const Ref<Object>& ref) {
    ref->SetName(j.at("name").get<std::string>());
    ref->SetUuid(j.at("uuid").get<std::string>());
    LOGGER(Log::Logger().Trace("Object %s uuid=%s", ref->GetName().c_str(), ref->GetUuid().c_str()));
}

void SchemaBuilder::BuildMesh(IResourceManager*const& resourceManager, const Json & json) {
    MeshRef mesh;
    std::string generator = json.at("generator");

    LOGGER(Log::Logger().Info("--- Invoke generator %s", generator.c_str()));
    if (generator.compare("cube") == 0)
    {
        mesh = GrafkitData::CreateCube();
    }
    //else if (generator.compare("cube") == 0)
    //{
    //}

    if (!mesh)
    THROW_EX_DETAILS(SchemaParseException, "Could not add mesh");
    BuildObject(json, mesh);
    resourceManager->Add(new Resource<Mesh>(mesh, mesh->GetName(), mesh->GetUuid()));
}

void SchemaBuilder::BuildAssets(IResourceManager*const& resourceManager, const Json& json) {
    LOGGER(Log::Logger().Info("- Loading json"));

    // --- 
    const Json meshes = json["meshes"];
    if (!meshes.empty())
    {
        LOGGER(Log::Logger().Info("-- Loading meshes"));
        for (Json::const_iterator it = meshes.begin(); it != meshes.end(); ++it)
        {
            BuildMesh(resourceManager, *it);
        }
    }


    // --- 
    const Json models = json["models"];
    if (!models.empty())
    {
        LOGGER(Log::Logger().Info("-- Loading meshes"));
        for (Json::const_iterator it = models.begin(); it != models.end(); ++it)
        {
            ModelRef model = new Model();
            BuildObject(it.value(), model);
            resourceManager->Add(new Resource<Model>(model, model->GetName(), model->GetUuid()));
        }

    }

    // --- 
    const Json materials = json["materials"];
    if (!materials.empty())
    {
        LOGGER(Log::Logger().Info("-- Loading materials"));
        for (Json::const_iterator it = materials.begin(); it != materials.end(); ++it)
        {
            MaterialRef material = new Material();
            BuildObject(it.value(), material);
            resourceManager->Add(new Resource<Material>(material, material->GetName(), material->GetUuid()));
        }

    }

    AssignAssets(resourceManager, json);

}

void SchemaBuilder::AssignModel(IResourceManager*const& resourceManager, const Json &json) {
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

void SchemaBuilder::AssignAssets(IResourceManager*const& resourceManager, const Json& assets) {
    const Json models = assets["models"];
    if (!models.empty())
    {
        LOGGER(Log::Logger().Info("-- Assign models"));
        for (Json::const_iterator it = models.begin(); it != models.end(); ++it)
        {
            AssignModel(resourceManager, *it);
        }
    }
}

void SchemaBuilder::ExtractActorParent(IResourceManager*const& resourceManager, const Json& json, const ActorRef& actorRef) {

    const Json parent = json["parent"];
    if (!parent.empty() && !parent.is_null())
    {
        Ref<Resource<Actor>> parentRes = resourceManager->GetByUuid<Resource<Actor>>(parent.get<std::string>());
        if (parentRes.Valid() && parentRes->Valid())
        {
            LOGGER(Log::Logger().Info("--- Parent"));
            (*parentRes)->AddChild(actorRef);
        }
        else
        {
            LOGGER(Log::Logger().Warn("--- No Parent found %s", parent.get<std::string>().c_str()));
        }
    }
}

void SchemaBuilder::ExtractEntities(IResourceManager*const& resourceManager, const Json& json, ActorRef& actorRef) {
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

void SchemaBuilder::BuildSceneGraphs(IResourceManager*const& resourceManager, const Json& json) {
    LOGGER(Log::Logger().Info("- Loading scenegraphs"));
    if (!json.empty())
    {
        for (Json::const_iterator it = json.begin(); it != json.end(); ++it)
        {
            SceneGraphRef scenegraph = new SceneGraph();
            BuildObject(*it, scenegraph);

            resourceManager->Add(new Resource<SceneGraph>(scenegraph, scenegraph->GetName(), scenegraph->GetUuid()));

            const Json actors = (*it)["actors"];
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
        }
    }
}

void SchemaBuilder::BuildEffects(const Json& effects, IResourceManager*& resourceManager) {
}
