#include "../stdafx.h"

#include "ResourceManager.h"
#include "ResourceBuilder.h"

using namespace Grafkit;
using namespace FWdebugExceptions;

IResourceManager::IResourceManager()
{
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

		m_resources[pResource->GetName()] = pResource;
	}	
}

void IResourceManager::Remove(const std::string & pName)
{
	ResourceMap_t::iterator it = m_resources.find(pName);
	if (it != m_resources.end()) {
		m_resources.erase(it); // feltehetoleg a resouece manager torli maga utan a ptr-t
	}
}

void Grafkit::IResourceManager::Load(IResourceBuilder * builder)
{	
	ResourceMap_t::const_iterator it = m_resources.find(builder->GetName());
	
	// Ha nincs resource, elotoltjuk. 
	if (it == m_resources.end()) {
		Reload(builder);
	}
}

void Grafkit::IResourceManager::Reload(IResourceBuilder * builder)
{
	IResource *resource = builder->NewResource();
	resource->SetName(builder->GetName());
	Add(resource);
	m_builders.push_back(builder);
}

void Grafkit::IResourceManager::DoPrecalc()
{
	size_t i = 0;
	size_t len = m_builders.size();

	// an event before loader

	for (BuilderList_t::iterator it = m_builders.begin(); it != m_builders.end(); it++) {
		LOGGER(Log::Logger().Trace("Preloading item %d of %d", i, len));
		if (*it) {
			(*it)->Load(this, Get<IResource>((*it)->GetName()));
		}
		// event after item had loaded
		
		i++;
	}

	// an event after loader

	ClearLoadStack();
}

void Grafkit::IResourceManager::ClearLoadStack()
{
	for (BuilderList_t::iterator it = m_builders.begin(); it != m_builders.end(); it++) {
		if (*it) {
			delete (*it);
		}
	}

	m_builders.clear();
}
