#pragma once

#include <string>
#include <map>
#include <list>

#include "../render/renderer.h"

#include "resource.h"
#include "ResourceBuilder.h"

namespace Grafkit {

	class IAssetFactory;

	class IPreloadEvents {
	public:
		//virtual void OnBeginLoad() = 0;
		//virtual void OnBeforeElemLoad(IResourceBuilder *&builder, IResource *&res) = 0;
		virtual void OnElemLoad(size_t actual, size_t count) = 0;
		//virtual void OnEndLoad() = 0;
	};

	class IResourceManager
	{
	public:
		IResourceManager();
		~IResourceManager();

		template<class T> Ref<T> Get(const std::string &pName) const;

		void Add(Ref<IResource> pResource);
		void Remove(const std::string &pName);
		void RemoveAll();

		void SetPreloadListener(IPreloadEvents* listener) { m_preloadEvents = listener; }
		
		// preloader
		void Load(IResourceBuilder* builder);
		template<class T> Ref<T> Load(IResourceBuilder* builder);

		void Reload(IResourceBuilder* builder);

		virtual void DoPrecalc();

		void ClearLoadStack();

		virtual Renderer &GetDeviceContext() = 0;	///@todo, ha kell, akkor egy makroval ki kell venni
		virtual IAssetFactory *GetAssetFactory() = 0;

		/**
			Resource tipusokra (shader, texture) regisztral egy relativ eleresi utvonalat 
		*/
		void AddResourcePath(std::string resourceClass, std::string path);
		std::string GetResourcePath(std::string resourceClass);

	protected:
		std::map<std::string, std::string> m_pathMap;
		std::map<std::string, Ref<IResource>> m_resources;
		std::list<IResourceBuilder*> m_builders;

	protected: 
		IPreloadEvents* m_preloadEvents;
	};

	template<class T> inline Ref<T> IResourceManager::Get(const std::string &pName) const {
		auto it = m_resources.find(pName);

		if (it != m_resources.end()) {
			return Ref<T>(dynamic_cast<T*>(it->second.Get()));
		}
		else
			return nullptr;
	}
	
	template<class T> inline Ref<T> IResourceManager::Load(IResourceBuilder* builder) {
		Load(builder);
		T* res = Get<T>(builder->GetName());
		return res;
	}

}

DEFINE_EXCEPTION(NoAssetFoundByNameException, 1, "No asset found by the given name");
DEFINE_EXCEPTION(NoAssetBucketFoundException, 3, "No asset bucket found");
DEFINE_EXCEPTION(UpdateResourceExcpetion, 4, "Cannot update resource");