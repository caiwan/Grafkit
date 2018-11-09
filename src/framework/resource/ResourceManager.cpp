#include "stdafx.h"



#include "ResourceManager.h"
#include "ResourceBuilder.h"

#include <iterator>
#include <sstream>

DEFINE_EXCEPTION(NoAssetFoundByNameException, 1, "No asset found by the given name");
DEFINE_EXCEPTION(NoAssetBucketFoundException, 3, "No asset bucket found");
DEFINE_EXCEPTION(UpdateResourceExcpetion, 4, "Cannot update resource");

using namespace Grafkit;
using namespace FWdebugExceptions;

void IResourceManager::AddResourcePaths()
{
    /* Base resource paths inside the asset directory */
    SetResourcePath("texture", "textures/");
    SetResourcePath("shader", "shaders/");
    SetResourcePath("shaderincludesystem", "shaders/lib/");
    SetResourcePath("shaderincludelocal", "shaders/");
    SetResourcePath("model", "models/");
    SetResourcePath("syncdata", "sync/");
    SetResourcePath("animation", "animation/");
    SetResourcePath("scripts", "script/");
}

IResourceManager::IResourceManager() : m_preloadEvents(nullptr) { AddResourcePaths(); }

IResourceManager::IResourceManager(IPreloadEvents* preloadEvents)
    : m_preloadEvents(preloadEvents) {
    AddResourcePaths();
}

IResourceManager::~IResourceManager()
{
    ClearLoadStack();
    for (auto it = m_uuidMap.begin(); it != m_uuidMap.end(); ++it) { it->second.AssingnRef(nullptr); }
}

void IResourceManager::Add(const Ref<IResource> & pResource)
{
    if (pResource.Valid())
    {
        Ref<IResource> i_ref = GetByUuid<IResource>(pResource->GetUuid());
        if (i_ref.Valid())
        {
            if (i_ref.Get() != pResource.Get())
            {
                std::ostringstream msg;
                msg << "(A resource pointerek nem egyformak, vangy nem egyedi a UUID.Geteld ki elobb, aztan frissits.)";
                msg << " requested ref uuid = " << std::string(pResource->GetUuid()).c_str();
                msg << " stored ref uuid = " << std::string(i_ref->GetUuid()).c_str();
                THROW_EX_DETAILS(UpdateResourceExcpetion, msg.str().c_str());
            }
        }
        std::string uuid = pResource->GetUuid();

        m_uuidMap[uuid] = pResource;
    }
}

void IResourceManager::RemoveByUuid(const std::string& uuid)
{
    auto it = m_uuidMap.find(uuid);
    if (it != m_uuidMap.end())
    {
        std::string name = it->second->GetName();

        m_uuidMap.erase(it);

        transform(name.begin(), name.end(), name.begin(), tolower);
    }
}

void IResourceManager::RemoveAll() { m_uuidMap.clear(); }

void IResourceManager::GetAllResources(std::list<Ref<IResource>>& target) const {
    transform(m_uuidMap.begin(), m_uuidMap.end(), back_inserter(target), [](const std::map<std::string, Ref<IResource>>::value_type& value) { return value.second; });
}

void IResourceManager::Load(Ref<IResourceBuilder> builder)
{
    assert(builder);
    std::string uuid = builder->GetUuid();

    if (uuid.empty())
    {
        Reload(builder);
        return;
    }

    transform(uuid.begin(), uuid.end(), uuid.begin(), tolower);
    auto it = m_uuidMap.find(uuid);

    if (it == m_uuidMap.end()) { Reload(builder); }
}

// TODO: This needs to be  or at least the testcase 
void IResourceManager::TriggerReload(std::string filename)
{
    //TODO: this part stinks
    auto it = m_filenamesToBuilder.find(filename);
    if (it != m_filenamesToBuilder.end())
    {
        auto values = it->second;

        for (auto value : values)
        {
            IResourceBuilder* builder = value.second;
            IResource* oldResource = GetByUuid<IResource>(value.first);
            if (oldResource && builder)
            {
                LOGGER(Log::Logger().Trace("Reloading modified resource %s => %s", it->first.c_str(), value.first.c_str()));
                builder->Load(this, oldResource);
                Add(oldResource);
            }
        }
    }
}

void IResourceManager::Reload(Ref<IResourceBuilder> builder)
{
    Ref<IResource> resource = builder->NewResource();
    Uuid uuid(builder->GetUuid());
    if (!uuid.IsEmpty()) { resource->SetUuid(uuid); }
    resource->SetName(builder->GetName());
    Add(resource);
    m_builders[uuid] = builder;
    if (!builder->GetWatcherFilename().empty())
    {
        std::string name = builder->GetWatcherFilename();
        BuilderPair builderPair(uuid, builder);
        auto it = m_filenamesToBuilder.find(name);
        if (it != m_filenamesToBuilder.end()) { it->second.push_back(builderPair); }
        else
        {
            // TODO: get rid of this && make a bit more efficient this part || clean it up at least
            m_filenamesToBuilder[name] = std::list<BuilderPair>();
            m_filenamesToBuilder[name].push_back(builderPair); // lol?
        }
    }
}

void IResourceManager::DoPrecalc()
{
    size_t i = 0;
    size_t len = m_builders.size();

    if (m_preloadEvents)
        m_preloadEvents->OnBeginLoad();

    // pass 1
    for (auto it = m_builders.begin(); it != m_builders.end(); ++it)
    {
        LOGGER(Log::Logger().Trace("Preloading item %d of %d", i, len));
        IResourceBuilder* builder = it->second;
        if (builder) { builder->Load(this, GetByUuid<IResource>(builder->GetUuid())); }

        if (m_preloadEvents)
            m_preloadEvents->OnElemLoad(i, len * 2);

        i++;
    }

    i = 0;

    // pass 2
    for (auto it = m_builders.begin(); it != m_builders.end(); ++it)
    {
        LOGGER(Log::Logger().Trace("Initializing item %d of %d", i, len));
        IResourceBuilder* builder = it->second;
        if (builder) { builder->Initialize(GetDeviceContext(), this, GetByUuid<IResource>(builder->GetUuid())); }

        if (m_preloadEvents)
            m_preloadEvents->OnElemLoad(len + i, len * 2);

        i++;
    }

    if (m_preloadEvents)
        m_preloadEvents->OnEndLoad();

    ClearLoadStack();
}

void IResourceManager::ClearLoadStack()
{
    m_builders.clear();
}

void IResourceManager::SetResourcePath(std::string resourceType, std::string path) { m_pathMap[resourceType] = path; }

std::string IResourceManager::GetResourcePath(std::string resourceClass)
{
    auto it = m_pathMap.find(resourceClass);
    if (it != m_pathMap.end())
        return it->second;
    return std::string();
}
