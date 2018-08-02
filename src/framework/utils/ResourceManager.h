#pragma once

#include <algorithm>
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
        virtual void OnBeginLoad() {}
        virtual void OnElemLoad(size_t actual, size_t count) {}
        virtual void OnEndLoad() {}
    };

    class IResourceManager
    {
    public:
        IResourceManager();
        virtual ~IResourceManager();

        template<class T> Ref<T> GetByName(const std::string &pName) const;
        template<class T> Ref<T> GetByUuid(const std::string &uuid) const;

        void Add(Ref<IResource> pResource);
        void RemoveByName(const std::string &name);
        void RemoveByUuid(const std::string &uuid);
        void RemoveAll();

        void GetAllResources(std::list<Ref<IResource>> &resources) const;

        void SetPreloadListener(IPreloadEvents* listener) { m_preloadEvents = listener; }

        // preloader
        void Load(IResourceBuilder* builder);
        template<class T> Ref<T> Load(IResourceBuilder* builder);

        void TriggerReload(std::string filename);
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
        std::map<std::string, Ref<IResource>> m_nameMap;
        std::map<std::string, Ref<IResource>> m_uuidMap;
        std::map<std::string, IResourceBuilder*> m_builders;
        std::map<std::string, std::pair<std::string, IResourceBuilder*>> m_filenamesToBuilder;

        IPreloadEvents* m_preloadEvents;
    };

    template<class T>
    Ref<T> IResourceManager::GetByName(const std::string &pName) const
    {
        std::string name = pName;
        transform(name.begin(), name.end(), name.begin(), tolower);

        auto it = m_nameMap.find(name);

        if (it != m_nameMap.end()) {
            return Ref<T>(dynamic_cast<T*>(it->second.Get()));
        }

        return nullptr;
    }

    template<class T>
    Ref<T> IResourceManager::GetByUuid(const std::string &uuid) const
    {
        auto it = m_uuidMap.find(uuid);

        if (it != m_uuidMap.end()) {
            return Ref<T>(dynamic_cast<T*>(it->second.Get()));
        }

        return nullptr;
    }

    template<class T>
    Ref<T> IResourceManager::Load(IResourceBuilder* builder) {
        Load(builder);
        T* res = GetByName<T>(builder->GetName());
        return res;
    }

}

DEFINE_EXCEPTION(NoAssetFoundByNameException, 1, "No asset found by the given name");
DEFINE_EXCEPTION(NoAssetBucketFoundException, 3, "No asset bucket found");
DEFINE_EXCEPTION(UpdateResourceExcpetion, 4, "Cannot update resource");
