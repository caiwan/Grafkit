#include "stdafx.h"

#include "ResourceManager.h"
#include "ResourceBuilder.h"

#include <iterator>

using namespace Grafkit;
using namespace FWdebugExceptions;

IResourceManager::IResourceManager() : m_preloadEvents(nullptr)
{
    /* Base resource paths inside the asset directory */
    this->AddResourcePath("texture", "textures/");
    this->AddResourcePath("shader", "shaders/");
    this->AddResourcePath("shaderincludesystem", "shaders/lib/");
    this->AddResourcePath("shaderincludelocal", "shaders/");
    this->AddResourcePath("model", "models/");
    this->AddResourcePath("syncdata", "sync/");
}

IResourceManager::~IResourceManager()
{
    ClearLoadStack();
    for (auto it = m_resources.begin(); it != m_resources.end(); ++it) { it->second.AssingnRef(nullptr); }
}

void IResourceManager::Add(Ref<IResource> pResource)
{
    if (pResource.Valid())
    {
        Ref<IResource> i_ref = this->Get<IResource>(pResource->GetName());
        if (i_ref.Valid())
        {
            if (i_ref.Get() != pResource.Get())
            {
                THROW_EX_DETAILS(UpdateResourceExcpetion, "(A resource pointerek nem egyformak. Geteld ki elobb, aztan frissits.)");
            }
        }
        // case sensitive Windows filesystem crap shit
        std::string name = pResource->GetName();
        transform(name.begin(), name.end(), name.begin(), tolower);

        m_resources[name] = pResource;
    }
}

void IResourceManager::Remove(const std::string& pName)
{
    std::string name = pName;
    transform(name.begin(), name.end(), name.begin(), tolower);
    auto it = m_resources.find(name);
    if (it != m_resources.end()) { m_resources.erase(it); }
}

void IResourceManager::RemoveAll() { m_resources.clear(); }

// STL components p115
struct get_second : std::unary_function<std::map<std::string, Ref<IResource>>::value_type, Ref<IResource>>
{
    Ref<IResource> operator()(const std::map<std::string, Ref<IResource>>::value_type& value) const { return value.second; }
};

void IResourceManager::GetAllResources(std::list<Ref<IResource>>& target) const { transform(m_resources.begin(), m_resources.end(), back_inserter(target), get_second()); }

void IResourceManager::Load(IResourceBuilder* builder)
{
    std::string name = builder->GetName();
    transform(name.begin(), name.end(), name.begin(), tolower);

    auto it = m_resources.find(name);

    // Ha nincs resource, elotoltjuk. 
    if (it == m_resources.end()) { Reload(builder); }
}

void IResourceManager::TriggerReload(std::string filename)
{
    // filename => {resource name, builder object}
    auto it = m_filenamesToBuilder.find(filename);
    if (it != m_filenamesToBuilder.end())
    {
        auto value = it->second;
        IResourceBuilder* builder = value.second;
        IResource* oldResource = Get<IResource>(value.first);
        if (oldResource && builder)
        {
            LOGGER(Log::Logger().Trace("Reloading modified resource %s => %s", it->first.c_str(), value.first.c_str()));
            builder->Load(this, oldResource);
            Add(oldResource);
        }
    }
}

void IResourceManager::Reload(IResourceBuilder* builder)
{
    IResource* resource = builder->NewResource();
    std::string name = builder->GetName();
    resource->SetName(name);
    Add(resource);
    m_builders[name] = builder;
    if (!builder->GetSourceName().empty()) { m_filenamesToBuilder[builder->GetSourceName()] = std::pair<std::string, IResourceBuilder*>(name, builder); }
}

void IResourceManager::DoPrecalc()
{
    size_t i = 0;
    size_t len = m_builders.size();

    if (m_preloadEvents)
        m_preloadEvents->OnBeginLoad();

    for (auto it = m_builders.begin(); it != m_builders.end(); ++it)
    {
        LOGGER(Log::Logger().Trace("Preloading item %d of %d", i, len));
        IResourceBuilder* builder = it->second;
        if (builder) { builder->Load(this, Get<IResource>(builder->GetName())); }

        if (m_preloadEvents)
            m_preloadEvents->OnElemLoad(i, len);

        i++;
    }

    if (m_preloadEvents)
        m_preloadEvents->OnEndLoad();

    ClearLoadStack();
}

void IResourceManager::ClearLoadStack()
{
    for (auto it = m_builders.begin(); it != m_builders.end(); ++it)
    {
        IResourceBuilder* builder = it->second;
        if (builder) { delete builder; }
    }

    m_builders.clear();
}

void IResourceManager::AddResourcePath(std::string resourceType, std::string path) { m_pathMap[resourceType] = path; }

std::string IResourceManager::GetResourcePath(std::string resourceClass)
{
    auto it = m_pathMap.find(resourceClass);
    if (it != m_pathMap.end())
        return it->second;
    return std::string();
}
