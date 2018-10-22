#pragma once

#include "json.hpp"

#include "core/reference.h"
#include "core/exceptions.h"
#include "core/asset.h"

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

        void LoadFromAsset(const Grafkit::StreamRef& asset, Grafkit::IResourceManager* resourceManager) const;
        DemoRef GetDemo() const;

    protected:

        DemoRef m_demo;
        Json m_json;
        bool m_inited;
    };
}

DEFINE_EXCEPTION(SchemaParseException, 4100, "Could not load Demo project");
