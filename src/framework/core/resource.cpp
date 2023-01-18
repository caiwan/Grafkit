#include "stdafx.h"

#include <chrono>

#include "resource.h"

using namespace Grafkit;
using namespace std::chrono;

// ---- 

IResource::IResource() : Object(){
}

IResource::IResource(const IResource& other): Referencable(other), Object(other)
{
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

IResource::IResource(const std::string& name) : Object(name)
{
}

IResource::IResource(const std::string& name, const std::string& uuid) : Object(name, uuid)
{
}

IResource::~IResource() {
}

void IResource::Serialize(Archive& ar) {
    assert(0);
}

