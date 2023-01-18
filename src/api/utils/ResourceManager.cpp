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
	IResource *resource = builder->NewResource();
	resource->SetName(builder->GetName());
	Add(resource);
	m_builders.push_back(builder);
}

void Grafkit::IResourceManager::DoPrecalc()
{
	for (BuilderList_t::iterator it = m_builders.begin(); it != m_builders.end(); it++) {
		if (*it) {
			(*it)->Load(this, Get<IResource>((*it)->GetName()));
		}
	}

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
