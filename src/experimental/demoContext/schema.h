#pragma once 

#include "utils/reference.h"
#include "utils/exceptions.h"
#include "json_fwd.hpp"
#include "utils/Asset.h"
#include "json.hpp"

using Json = nlohmann::json;

namespace GkDemo {
class Demo;

typedef Ref<Demo> DemoRef;

    class SchemaBuilder {
    public :
        SchemaBuilder();
        ~SchemaBuilder();

        //void LoadFromFile(const char * filename);
        void LoadFromAsset(const Grafkit::IAssetRef& asset, Grafkit::IResourceManager* resourceManager);
        void Initialize() const;
        Ref<Demo> GetDemo() const;

    protected:
        void Build(const Json &json, Grafkit::IResourceManager*& resourceManager);
        static void BuildObject(const Json &json, const Ref<Grafkit::Object>& ref);
        //void BuildObject(const Json &json, const Ref<Grafkit::Object> &ref);
        static void BuildAssets(const Json &assets, Grafkit::IResourceManager*& resourceManager);
        void BuildSceneGraphs(const Json & scenegraphs, Grafkit::IResourceManager*& resourceManager);
        void BuildEffects(const Json& effects, Grafkit::IResourceManager*& resourceManager);

    private:
        Ref<Demo> m_demo;
    };
}

DEFINE_EXCEPTION(SchemaParseException, 4100, "Could not load Demo schemantic");
