#include "stdafx.h"

#include "ResourceManager.h"
#include "ResourceBuilder.h"

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
}

void IResourceManager::Add(Ref<IResource> pResource)
{
	if (pResource.Valid()) {
		Ref<IResource> i_ref = this->Get<IResource>(pResource->GetName());
		if (i_ref.Valid()) {
			if (i_ref.Get() != pResource.Get()) {
				throw new EX_DETAILS(UpdateResourceExcpetion, "(A resource pointerek nem egyformak. Geteld ki elobb, aztan frissits.)");
			}
		}
		// FUCKING case sensitive Windows filesystem crap shit
		std::string name = pResource->GetName();
		std::transform(name.begin(), name.end(), name.begin(), ::tolower);

		m_resources[name] = pResource;
	}	
}

void IResourceManager::Remove(const std::string & pName)
{
	auto it = m_resources.find(pName);
	if (it != m_resources.end()) {
		m_resources.erase(it);
	}
}

void IResourceManager::RemoveAll() {
	m_resources.clear();
}

void Grafkit::IResourceManager::Load(IResourceBuilder * builder)
{	
	std::string name = builder->GetName();
	std::transform(name.begin(), name.end(), name.begin(), ::tolower);

	auto it = m_resources.find(name);
	
	// Ha nincs resource, elotoltjuk. 
	if (it == m_resources.end()) {
		Reload(builder);
	}
}

void Grafkit::IResourceManager::TriggerReload(std::string filename)
{
	// filename => {resource name, builder object}
	auto it = m_filenamesToBuilder.find(filename);
	if (it != m_filenamesToBuilder.end()) {
		auto value = it->second;
		IResourceBuilder * builder = value.second;
		IResource *oldResource = Get<IResource>(value.first);
		if (oldResource && builder) {
			LOGGER(Log::Logger().Trace("Reloading modified resource %s => %s", it->first.c_str(), value.first.c_str()));
			builder->Load(this, oldResource);
			Add(oldResource);
		}
	}
}

void Grafkit::IResourceManager::Reload(IResourceBuilder * builder)
{
	IResource *resource = builder->NewResource();
	std::string name = builder->GetName();
	resource->SetName(name);
	Add(resource);
	m_builders[name] = builder;
	if (!builder->GetSourceName().empty()) {
		m_filenamesToBuilder[builder->GetSourceName()] = std::pair<std::string, IResourceBuilder*>(name, builder);
	}
}

void Grafkit::IResourceManager::DoPrecalc()
{
	size_t i = 0;
	size_t len = m_builders.size();

	// an event before loader

	for (auto it = m_builders.begin(); it != m_builders.end(); it++) {
		LOGGER(Log::Logger().Trace("Preloading item %d of %d", i, len));
		IResourceBuilder * builder = it->second;
		if (builder) {
			builder->Load(this, Get<IResource>(builder->GetName()));
		}

		if (m_preloadEvents)
			m_preloadEvents->OnElemLoad(i, len);
		// event after item had loaded
		
		i++;
	}

	// an event after loader

	ClearLoadStack();
}

void Grafkit::IResourceManager::ClearLoadStack()
{
#ifdef LIVE_RELEASE
	for (auto it = m_builders.begin(); it != m_builders.end(); it++) {
		IResourceBuilder * builder = it->second;
		if (builder) {
			delete builder;
		}
}

	m_builders.clear();
#endif // !LIVE_RELEASE
}

void Grafkit::IResourceManager::AddResourcePath(std::string resourceType, std::string path)
{
	m_pathMap[resourceType] = path;
}

std::string Grafkit::IResourceManager::GetResourcePath(std::string resourceClass)
{
	auto it = m_pathMap.find(resourceClass);
	if (it != m_pathMap.end())
		return it->second;
	return std::string();
}
