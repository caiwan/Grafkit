#include "stdafx.h"

#include <chrono>

#include "resource.h"

using namespace Grafkit;
using namespace std::chrono;

// Quick and dirty crap
// to clamp down values to create seed and random
template <typename T, typename U>
U crawl(T in)
{
    U out = 0;

    const int sIn = sizeof(in), sOut = sizeof(out);

    BYTE* pOut = static_cast<BYTE*>(&out);
    const BYTE* pIn = static_cast<BYTE*>(&in);

    for (int i = 0; i < std::max(sIn, sOut); ++i) { pOut[i % sOut] ^= pIn[i % sIn]; }

    return out;
}

// ---- 

IResource::IResource() {
    CreateUuid();
}

IResource::IResource(const IResource& other): Referencable(other)
    , m_name(other.m_name)
    , m_uuid(other.m_uuid) {
}

IResource& IResource::operator=(const IResource& other)
{
    if (this == &other)
        return *this;
    Referencable::operator =(other);
    m_name = other.m_name;
    m_uuid = other.m_uuid;
    return *this;
}

IResource::IResource(const std::string& name) : m_name(name) 
{
    CreateUuid();
}

IResource::IResource(const std::string& name, const std::string& uuid) :
    m_name(name)
    , m_uuid(uuid) 
{
}

IResource::~IResource() {
}

#pragma comment(lib, "rpcrt4.lib")  // UuidCreate - Minimum supported OS Win 2000
void IResource::CreateUuid()
{
    UUID uuid;
    UuidCreate(&uuid);
    char *str;
    UuidToStringA(&uuid, reinterpret_cast<RPC_CSTR*>(&str));
    m_uuid.assign(str);
    RpcStringFreeA(reinterpret_cast<RPC_CSTR*>(&str));
}
