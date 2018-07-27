#pragma once 

#include "utils/reference.h"
#include "utils/exceptions.h"
#include "json_fwd.h"

using Json = nlohmann::json;

namespace GkDemo {
class Demo;

typedef Ref<Demo> DemoRef;

    class SchemaBuilder {
    public :
        SchemaBuilder();
        ~SchemaBuilder();

        void LoadFromFile(const char * filename);
        void Initialize();

    protected:
        void Build(const Json &json);

    private:
        Demo * m_demo;
    };
}

DEFINE_EXCEPTION(SchemaParseException, 4100, "Could not load Demo schemantic");
