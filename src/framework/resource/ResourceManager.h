#pragma once


#include <map>
#include <list>

#include "common.h"

#include "render/renderer.h"
#include "core/Resource.h"
#include "ResourceBuilder.h"

namespace Grafkit
{
    class IAssetFactory;

    class IResourceLoader;
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

#if 0
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

#endif

    class ResourceManager
    {
    public:
        
        ResourceManager(/*Renderer& render, IAssetFactory& assetFactory, */IPreloadEvents* preloadEvents = nullptr) :
            /*m_render(render)
            , m_assetFactory(assetFactory),*/
            m_preloadEvents(preloadEvents) {
        }

        ResourceManager(const ResourceManager& other) = delete;
        ResourceManager& operator=(const ResourceManager& other) = delete;

        template <class T>
        Resource<T> Load(const std::shared_ptr<IResourceLoader>& loader);

        void Reload(const std::shared_ptr<IResourceLoader>& loader);
        void Reload(const Uuid& id);
        void Add(const std::shared_ptr<IResource>& resource) { m_resources[resource->GetId()] = resource; }
        void Remove(const std::shared_ptr<IResource>& resource);
        void DoPrecalc(Renderer& render, IAssetFactory& assetFactory);

        template <class T>
        Resource<T> Get(const Uuid& id) const;

        template <class R>
        std::shared_ptr<R> GetLoader(const Uuid& id) const;

        void SetPreloadEvents(IPreloadEvents* const &preloadEvents) { m_preloadEvents = preloadEvents; }

        //Renderer& GetDeviceContext() const { return m_render; }
        //IAssetFactory& GetAssetFactory() const { return m_assetFactory; }

    protected:
        std::shared_ptr<IResource> GetRaw(const Uuid& id) const;
        std::shared_ptr<IResourceLoader> GetLoaderRaw(const Uuid& id) const;

        template <class T>
        std::shared_ptr<ResourceWrapper<T>> CreateResource(const std::shared_ptr<IResourceLoader>& loader);

    private:
        std::map<Uuid, std::shared_ptr<IResource>> m_resources;
        std::map<Uuid, std::shared_ptr<IResourceLoader>> m_loaders;
        std::list<std::shared_ptr<IResourceLoader>> m_loaderQueue;

        // --- 

        //Renderer& m_render;
        //IAssetFactory& m_assetFactory;

        IPreloadEvents *m_preloadEvents;
    };

    // ====================================================================================================================

    template <class T>
    Resource<T> ResourceManager::Load(const std::shared_ptr<IResourceLoader>& loader)
    {
        assert(loader);

        auto resource = Get<T>(loader->GetId());
        if (!resource.Empty())
            return resource;

        auto ptr = CreateResource<T>(loader);
        Add(ptr);
        Reload(loader);
        return Resource<T>(std::move(ptr));
    }

    template <class T>
    Resource<T> ResourceManager::Get(const Uuid& id) const
    {
        auto ptr = GetRaw(id);
        if (ptr)
        {
            auto tid = ptr->GetTypeIndex();
            auto myTid = std::type_index(typeid(T));
            // tid is base of T
            if (tid == myTid) { return Resource<T>(std::dynamic_pointer_cast<ResourceWrapper<T>>(ptr)); }
            // tid is base of T or tid is derived from T
            if (tid > myTid || tid < myTid)
            {
                ResourceWrapper<T>* myPtr = reinterpret_cast<ResourceWrapper<T>*>(ptr.get());
                return Resource<T>(std::shared_ptr<ResourceWrapper<T>>(ptr, myPtr));
            }
        }
        return Resource<T>();
    }

    template <class R>
    std::shared_ptr<R> ResourceManager::GetLoader(const Uuid& id) const
    {
        auto ptr = GetLoaderRaw(id);
        if (ptr)
        {
            auto castPtr = std::dynamic_pointer_cast<R>(ptr);
            assert(castPtr);
            return castPtr;
        }
        return nullptr;
    }

    template <class T>
    std::shared_ptr<ResourceWrapper<T>> ResourceManager::CreateResource(const std::shared_ptr<IResourceLoader>& loader)
    {
        auto ptr = std::dynamic_pointer_cast<ResourceWrapper<T>>(loader->CreateResource());
        ptr->SetId(loader->GetId());
        return ptr;
    }
}
