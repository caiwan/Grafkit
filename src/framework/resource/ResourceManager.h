#pragma once


#include <map>
#include <list>

#include "common.h"

#include "render/renderer.h"
#include "ResourceBuilder.h"

namespace Grafkit
{
    class IAssetFactory;

    class IResourceBuilder;
    class IResource;

    class IPreloadEvents
    {
    public:
        virtual ~IPreloadEvents() = default;

        virtual void OnBeginLoad() {
        }

        virtual void OnElemLoad(size_t actual, size_t count) {
        }

        virtual void OnEndLoad() {
        }
    };

    class IResourceManager
    {
    public:
        void AddResourcePaths();
        IResourceManager();
        explicit IResourceManager(IPreloadEvents* preloadEvents);

        virtual ~IResourceManager();

        template <class T>
        Ref<T> GetByUuid(const std::string& uuid) const;

        void Add(const IResourceRef & pResource);
        void RemoveByUuid(const std::string& uuid);
        void RemoveAll();

        void GetAllResources(std::list<IResourceRef>& resources) const;

        void SetPreloadListener(IPreloadEvents* listener) { m_preloadEvents = listener; }

        // preloader
        void Load(Ref<IResourceBuilder> builder);
        template <class T>
        Ref<T> Load(Ref<IResourceBuilder> builder);

        void TriggerReload(std::string filename);
        void Reload(Ref<IResourceBuilder> builder);

        virtual void DoPrecalc();

        void ClearLoadStack();

        virtual Renderer& GetDeviceContext() = 0; ///@todo, ha kell, akkor egy makroval ki kell venni
        virtual IAssetFactory* GetAssetFactory() = 0;

        /**
            Resource tipusokra (shader, texture) regisztral egy relativ eleresi utvonalat
        */
        void SetResourcePath(std::string resourceClass, std::string path);
        std::string GetResourcePath(std::string resourceClass);

    protected:
        typedef std::pair<std::string, Ref<IResourceBuilder>> BuilderPair;
        std::map<std::string, std::string> m_pathMap; // TODO: this stinks
        std::map<std::string, Ref<IResource>> m_uuidMap;
        std::map<std::string, Ref<IResourceBuilder>> m_builders;
        std::map<std::string, std::list<BuilderPair>> m_filenamesToBuilder; // TODO: this stinks

        IPreloadEvents* m_preloadEvents;
    };


    template <class T>
    Ref<T> IResourceManager::GetByUuid(const std::string& uuid) const
    {
        auto it = m_uuidMap.find(uuid);
        if (it != m_uuidMap.end()) { return Ref<T>(dynamic_cast<T*>(it->second.Get())); }
        return nullptr;
    }

    template <class T>
    Ref<T> IResourceManager::Load(Ref<IResourceBuilder> builder)
    {
        Load(builder);
        T* res = GetByUuid<T>(builder->GetUuid());
        return res;
    }
}
