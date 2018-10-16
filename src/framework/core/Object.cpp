#include "stdafx.h"

#include "Object.h"

using namespace Grafkit;

#pragma comment(lib, "rpcrt4.lib")  // UuidCreate - Minimum supported OS Win 2000
std::string Uuid::Create() {
    UUID uuid;
    UuidCreate(&uuid);
    char* str;
    UuidToStringA(&uuid, reinterpret_cast<RPC_CSTR*>(&str));
    std::string uuidStr;
    uuidStr.assign(str);
    RpcStringFreeA(reinterpret_cast<RPC_CSTR*>(&str));
    return uuidStr;
}

// ... 

//void Object::_Serialize(Archive& ar)
//{
//    PERSIST_STRING(ar, m_uuid);
//    PERSIST_STRING(ar, m_name);
//}
