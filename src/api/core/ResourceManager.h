#pragma once

#include <string>
#include <map>

#include "resource.h"

#include "../render/renderer.h"

namespace Grafkit{

	class IAssetFactory;

	class IResourceManager
	{
	public:
		IResourceManager();
		~IResourceManager();

		template<class T> T* Get(const std::string &pName) const;

		void Add(IResource* pResource);

		void Add(const std::string &pName, IResource* pResource);
		void Remove(const std::string &pName);
		
		/// creates a new resource manager for a certain purpose, if needed 
		// IResourceManager* GetResoueceManager(std::string &pName);

		// ez kell mindenkeppen:
		virtual Renderer &GetDeviceContext() = 0;
		virtual IAssetFactory *GetAssetFactory() = 0;

	private:
		typedef std::map<std::string, IResourceRef> ResourceMap_t;
		ResourceMap_t m_resources;

		IResourceManager* m_pParent;
	};

	template<class T>
	inline Ref<T> IResourceManager::Get(const std::string &pName) const {
		ResourceMap::const_iterator it = m_resources.find(pName);

		if (it != m_resources.end()) {
			it->second->AddRef();
			return static_cast<T*>(it->second);
		}
		else
			return nullptr;
	}
	

}

DEFINE_EXCEPTION(NoAssetFoundByNameException, 1, "No asset found by the given name");
#ifndef _NO_UUID_
DEFINE_EXCEPTION(NoAssetFoundByUUIDException, 2, "No asset found by the given UUID");
#endif //_NO_UUID_
DEFINE_EXCEPTION(NoAssetBucketFoundException, 3, "No asset bucket found");


//*845912630,/*859462130,/*89512630,/*-895+236 0,0