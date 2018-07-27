#include <fstream>

#include "schema.h"
#include "json.hpp"

#include "utils/exceptions.h"
#include "utils/logger.h"

using namespace FWdebugExceptions;

using json = nlohmann::json;

GkDemo::SchemaBuilder::SchemaBuilder()
{
}

GkDemo::SchemaBuilder::~SchemaBuilder()
{
}

void GkDemo::SchemaBuilder::LoadFromFile(const char * filename)
{
    FILE *fp = nullptr;
    auto error = fopen_s(&fp, filename, "rb");
    if (error == S_OK) {
        fseek(fp, 0, SEEK_END);
        size_t len = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        char * data = (char*)malloc(len + 1);
        fread_s(data, len, 1, len, fp);
        fclose(fp);

        try {
            Json::parse(data);
        }
        catch (json::parse_error &e) {
            throw new EX_DETAILS(SchemaParseException, e.what());
        }
        catch (json::exception &e) {
            throw new EX_DETAILS(SchemaParseException, e.what());
        }
        catch (std::exception &e) {
            throw new EX_DETAILS(SchemaParseException, e.what());
        }
        catch (...) {
            throw new EX(SchemaParseException);
        }

        //Json j;
        //if (j.accept(data)) {
        //    Build(j);
        //}
        //else {
        //    j.is_discarded()
        //    //const char* msg = j.(error);
        //    throw new EX(SchemaParseException);
        //}

        free(data);
    }
    else {
        const char* msg = std::strerror(error);
        throw new EX_DETAILS(SchemaParseException, msg);
    }

}

void GkDemo::SchemaBuilder::Initialize()
{
}

void GkDemo::SchemaBuilder::Build(const Json & json)
{
    // build demo
    json["demo"];
}
