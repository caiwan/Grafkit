#pragma once

#include "json.hpp"

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

        void LoadFromAsset(const Grafkit::IAssetRef& asset, Grafkit::IResourceManager* resourceManager);
        void Initialize(Grafkit::IResourceManager*const& resourceManager);
        Ref<Demo> GetDemo() const;

    protected:
        void Build(Grafkit::IResourceManager*const& resourceManager, const Json& json);
        void BuildResources(Grafkit::IResourceManager*const& resourceManager, const Json& json);
        static void BuildAssets(Grafkit::IResourceManager*const& resourceManager, const Json& assets);
        static void BuildScene(Grafkit::IResourceManager* const & resourceManager, const Json& sceneJson);
        static void BuildAnimation(Grafkit::IResourceManager* const & resourceManager, const Json& sceneJson);

        static void BuildObject(const Json& json, const Ref<Grafkit::Object>& ref);

        static void BuildMesh(Grafkit::IResourceManager*const& resourceManager, const Json& json);
        static void BuildMaterial(Grafkit::IResourceManager* const & resourceManager, const Json& it);
        static void BuildShader(Grafkit::IResourceManager* const & resourceManager, const Json shader);
        void BuildSceneGraphs(Grafkit::IResourceManager*const& resourceManager, const Json& json);
        void BuildEffects(Grafkit::IResourceManager*const& resourceManager, const Json& effects);

        // --- 

        void AssignAnimations(Grafkit::IResourceManager*const& resourceManager, const Json& json);
        void AssignCamerasToScene(Grafkit::IResourceManager* const &resourceManager, const Json &sceneJson, const uint32_t &sceneId);
        void AssignShader(Grafkit::IResourceManager* const & resourceManager, Json sceneJson);

        void AssignModel(Grafkit::IResourceManager*const& resourceManager, const Json& json);
        void AssignAssets(Grafkit::IResourceManager*const& resourceManager, const Json& json);

        void ExtractActorParent(Grafkit::IResourceManager*const& resourceManager, const Json& json, const Grafkit::ActorRef& actorRef);
        void ExtractEntities(Grafkit::IResourceManager*const& resourceManager, const Json& json, Grafkit::ActorRef& actorRef);

    private:
        Ref<Demo> m_demo;
        Json m_json;
        bool m_inited;
    };
}

DEFINE_EXCEPTION(SchemaParseException, 4100, "Could not load Demo project");
