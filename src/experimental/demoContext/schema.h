#pragma once

#include "json_fwd.hpp"

#include "utils/reference.h"
#include "utils/exceptions.h"
#include "utils/Asset.h"

using Json = nlohmann::json;

namespace GkDemo
{
    class Demo;

    typedef Ref<Demo> DemoRef;

    class SchemaBuilder
    {
    public:
        SchemaBuilder();
        ~SchemaBuilder();

        //void LoadFromFile(const char * filename);
        void LoadFromAsset(const Grafkit::IAssetRef& asset, Grafkit::IResourceManager* resourceManager);
        void Initialize() const;
        Ref<Demo> GetDemo() const;

    protected:
        void Build(Grafkit::IResourceManager*const& resourceManager, const Json& json);
        void BuildResources(Grafkit::IResourceManager*const& resourceManager, const Json& json);

        void BuildScenes(Grafkit::IResourceManager * const & resourceManager, const Json & demo);

        void BuildAssets(Grafkit::IResourceManager*const& resourceManager, const Json& assets);
        void AssignModel(Grafkit::IResourceManager*const& resourceManager, const Json& json);

        void AssignAssets(Grafkit::IResourceManager*const& resourceManager, const Json& json);

        void ExtractActorParent(Grafkit::IResourceManager*const& resourceManager, const Json& json, const Grafkit::ActorRef& actorRef);
        void ExtractEntities(Grafkit::IResourceManager*const& resourceManager, const Json& json, Grafkit::ActorRef& actorRef);

        void BuildObject(const Json& json, const Ref<Grafkit::Object>& ref);

        void BuildMesh(Grafkit::IResourceManager*const& resourceManager, const Json& json);
        void BuildMaterial(Grafkit::IResourceManager * const & resourceManager, const Json & it);
        void BuildShader(Grafkit::IResourceManager * const & resourceManager, const Json shader);
        void BuildSceneGraphs(Grafkit::IResourceManager*const& resourceManager, const Json& json);
        void BuildEffects(Grafkit::IResourceManager*& resourceManager, const Json& effects);

    private:
        Ref<Demo> m_demo;
    };
}

DEFINE_EXCEPTION(SchemaParseException, 4100, "Could not load Demo project");
