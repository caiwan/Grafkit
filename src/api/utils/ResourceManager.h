#pragma once

#include <string>
#include <map>
#include <list>

#include "../render/renderer.h"

#include "resource.h"
#include "ResourceBuilder.h"

namespace Grafkit {

	class IAssetFactory;

	/// Ez lesz majd a betoltocsik
	class PreloadEvents {
	public:
		virtual void OnBeginLoad() = 0;
		virtual void OnElemLoad(size_t actual, size_t count) = 0;
		virtual void OnEndLoad() = 0;
	};

	class IResourceManager
	{
	public:
		IResourceManager();
		~IResourceManager();

		template<class T> Ref<T> Get(const std::string &pName) const;

		void Add(Ref<IResource> pResource);

		void Remove(const std::string &pName);
		
		// preloader
		void Load(IResourceBuilder* builder);
		template<class T> Ref<T> Load(IResourceBuilder* builder);

		void Reload(IResourceBuilder* builder);

		virtual void DoPrecalc();

		void ClearLoadStack();

		virtual Renderer &GetDeviceContext() = 0;	///@todo, ha kell, akkor egy makroval ki kell venni
		virtual IAssetFactory *GetAssetFactory() = 0;

	protected:

		typedef std::map<std::string, Ref<IResource>> ResourceMap_t;
		ResourceMap_t m_resources;

		// ResourceMap_t::const_iterator AddLoad(IResourceBuilder* builder);
		typedef std::list<IResourceBuilder*> BuilderList_t;
		BuilderList_t m_builders;
	};

	template<class T> inline Ref<T> IResourceManager::Get(const std::string &pName) const {
		ResourceMap_t::const_iterator it = m_resources.find(pName);

		if (it != m_resources.end()) {
			return Ref<T>(dynamic_cast<T*>(it->second.Get()));
		}
		else
			return nullptr;
	}
	
	template<class T> inline Ref<T> IResourceManager::Load(IResourceBuilder* builder) {
		Load(builder);
		return Get<T>(builder->GetName());
	}

}

DEFINE_EXCEPTION(NoAssetFoundByNameException, 1, "No asset found by the given name");
#ifndef _NO_UUID_
DEFINE_EXCEPTION(NoAssetFoundByUUIDException, 2, "No asset found by the given UUID");
#endif //_NO_UUID_
DEFINE_EXCEPTION(NoAssetBucketFoundException, 3, "No asset bucket found");

DEFINE_EXCEPTION(UpdateResourceExcpetion, 4, "Cannot update resource");