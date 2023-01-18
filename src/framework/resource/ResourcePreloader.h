#pragma once

#include <vector>

#include "ResourceManager.h"


namespace Grafkit{
	
    class AssetFileFilter;

    class IPreloadEvents;

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

        virtual void LoadCache();
        virtual void SaveCache();

    //protected:
        //std::vector<AssetFileFilter*> m_filters;
    };


}
