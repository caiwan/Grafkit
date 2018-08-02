#include <fstream>

#include "schema.h"
#include "demo.h"
#include "json.hpp"

#include "render/mesh.h"

#include "utils/asset.h"
#include "utils/ResourceManager.h"

#include "utils/exceptions.h"
#include "utils/logger.h"

#include "builtin_data/cube.h"

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

    Json j;
    const char * dataPtr = static_cast<const char*>(asset->GetData());
    std::string data(dataPtr, dataPtr + asset->GetSize());
#if 1

    try {
        j = Json::parse(data);
        Build(j, resourceManager);
    }
    catch (std::exception &e) {
        THROW_EX_DETAILS(SchemaParseException, e.what());
    }
#else

    if (!j.accept(data))
    //j.is_discarded()) 
    {
        THROW_EX_DETAILS(SchemaParseException, "Failed to parse json, check syntax");
    }
    Build(j, resourceManager);
#endif

}

void SchemaBuilder::Initialize() const {
}

Ref<Demo> SchemaBuilder::GetDemo() const { return m_demo; }

void SchemaBuilder::Build(const Json& json, IResourceManager*& resourceManager) {
    LOGGER(Log::Logger().Info("Loading context"));

    // build demo
    //Json assets = ;
    //if (!assets.empty())
        BuildAssets(json.at("assets"), resourceManager);
    //else
        //THROW_EX_DETAILS(SchemaParseException, "No assets");

    //Json scenegraphs = json["scenegraphs"];
    //if (!scenegraphs.empty())
    //    BuildSceneGraphs(scenegraphs, resourceManager);
    //else
    //    THROW_EX_DETAILS(SchemaParseException, "No scenegraphs");

    //Json effects = json["effects"];
    //if (!effects.empty())
    //    BuildEffects(effects, resourceManager);
    //else
    //    THROW_EX_DETAILS(SchemaParseException, "no effects");

    //// DAMN
    //Json demo = json["demo"];
    //if (!effects.empty())
    //{
    //    // Do the magic
    //}
    //else
    //    THROW_EX_DETAILS(SchemaParseException, "Failed to parse json, check syntax");

}

void SchemaBuilder::BuildObject(const Json& j, const Ref<Grafkit::Object>& ref) {
    ref->SetName(j.at("name").get<std::string>());
    ref->SetUuid(j.at("uuid").get<std::string>());
    LOGGER(Log::Logger().Info("Object %s uuid=%s", ref->GetName().c_str(), ref->GetUuid().c_str()));
}

void SchemaBuilder::BuildAssets(const Json& assets, IResourceManager*& resourceManager) {
    LOGGER(Log::Logger().Info("- Loading assets"));

    Json meshes = assets["meshes"];
    if (!meshes.empty())
    {
        LOGGER(Log::Logger().Info("-- Loading meshes"));
        for (Json::iterator it = meshes.begin(); it != meshes.end(); ++it)
        {
            MeshRef mesh;
            std::string generator = it.value().at("generator");

            LOGGER(Log::Logger().Info("--- Invoke generator %s", generator.c_str()));
            if (generator.compare("cube") == 0)
            {
                mesh = GrafkitData::CreateCube();
            }

            if (!mesh)
                THROW_EX_DETAILS(SchemaParseException, "Could not add mesh");

            BuildObject(it.value(), mesh);

            resourceManager->Add(new Resource<Mesh>(mesh, mesh->GetName(), mesh-> GetUuid()));
        }
    }

}

void SchemaBuilder::BuildSceneGraphs(const Json& scenegraphs, IResourceManager*& resourceManager) {
}

void SchemaBuilder::BuildEffects(const Json& effects, IResourceManager*& resourceManager) {
}
