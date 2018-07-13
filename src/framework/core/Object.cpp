#include "stdafx.h"

#include "Object.h"

using namespace Grafkit;

Object::Object() : Referencable()
    , Persistent() { CreateUuid(); }

Object::Object(const std::string& name) : Referencable()
    , Persistent()
    , m_name(name) { CreateUuid(); }

Object::Object(const std::string& name, const std::string& uuid) : Referencable()
    , Persistent()
    , m_name(name)
    , m_uuid(uuid) {
}

Object::~Object() {
}

Object::Object(const Object& other) : Referencable()
    , Persistent()
    , m_name(other.m_name)
    , m_uuid(other.m_uuid) {
}

// ... 
#pragma comment(lib, "rpcrt4.lib")  // UuidCreate - Minimum supported OS Win 2000
void Object::CreateUuid()
{
    UUID uuid;
    UuidCreate(&uuid);
    char* str;
    UuidToStringA(&uuid, reinterpret_cast<RPC_CSTR*>(&str));
    m_uuid.assign(str);
    RpcStringFreeA(reinterpret_cast<RPC_CSTR*>(&str));
}

void Object::_Serialize(Archive& ar)
{
    PERSIST_STRING(ar, m_uuid);
    PERSIST_STRING(ar, m_name);
}
