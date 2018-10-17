#pragma once

#include <vector>

#include "ResourceManager.h"
#include "ResourceBuilder.h"

namespace Grafkit
{
    class IPreloadEvents;
    class AssetFileFilter;

    /*Legacy code*/
    class ResourcePreloader : public IResourceManager
    {
    public:
        explicit ResourcePreloader(IPreloadEvents* pPreloader = nullptr);
        virtual ~ResourcePreloader();

        virtual void LoadCache();
        virtual void SaveCache();

        protected:
        	std::vector<AssetFileFilter*> m_filters;
    };

    /*
     *
     */
    class ParametricResourceLoader : public IResourceManager
    {
    protected:
        typedef std::function<Ref<IResourceBuilder>()> ResourceBuilderFactroy;
    public:
        explicit ParametricResourceLoader(IPreloadEvents* pPreloader = nullptr);
        virtual ~ParametricResourceLoader();

        // Fuck all of this crap

        //template<class B>
        //void RegisterBuilderFactory(const char* builderName)
        //{
        //    assert(m_factories.find(builderName) == m_factories.end());
        //    m_factories[builderName] = []() {return new B(); };
        //}

        virtual void LoadCache();
        virtual void SaveCache();

        //template<class T >
        //void PushNewBuilder(std::string builderName, std::string name, const Uuid uuid, const T& params)
        //{
        //    auto it = m_factories.find(builderName);
        //    if (it != m_factories.end())
        //    {
        //        const Ref<IResourceBuilder> builder = it->second();
        //        //builder->SetName(name);
        //        //builder->SetUuid(uuid);
        //        builder->SetParamP(/*reinterpret_cast<const void*>*/(&params));
        //        Load(builder);
        //    }
        //}

        //template<class T >
        //void PushNewBuilder(std::string builderName, const T& params)
        //{
        //    auto it = m_factories.find(builderName);
        //    if (it != m_factories.end())
        //    {
        //        auto *builder = it->second();
        //        builder->SetParamP(&p);
        //        Load(builder);
        //    }
        //}

    private:
        //std::map<std::string, ResourceBuilderFactroy> m_factories;
    };
}
