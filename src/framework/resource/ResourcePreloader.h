#pragma once

#include <vector>

#include "ResourceManager.h"
//#include "ResourceBuilder.h"

namespace Grafkit
{
    class IResourceBuilderFactroy;

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

    class ParametricResourceLoader : public IResourceManager
    {
    public:
        explicit ParametricResourceLoader(IPreloadEvents* pPreloader = nullptr);
        virtual ~ParametricResourceLoader();

        //void RegisterBuilderFactory(IResourceBuilderFactroy* factroy);

        virtual void LoadCache();
        virtual void SaveCache();

        //template<class typename T >
        //void PushNewBuilder(std::string builder, std::string name, std::string uuid, const T& params)
        //{
        //    auto it = m_factories.find(builder);
        //    if (it != m_factories.end())
        //    {
        //        Load(it->second->NewBuilder());
        //    }
        //}

    private:
        std::map<std::string, IResourceBuilderFactroy*> m_factories;
    };
}
