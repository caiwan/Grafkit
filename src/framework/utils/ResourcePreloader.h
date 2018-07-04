#pragma once

#include <vector>

#include "ResourceManager.h"
#include "AssetFactory.h"

namespace Grafkit{
	/**
	Ez egy olyan dolog, ami automatikusan betolt, es cachel resourcokat
	Dirty hack of mine
	Ide fogom behackelni a livereload managert is 
	*/
	class ResourcePreloader : public IResourceManager
	{
	public:
	    explicit ResourcePreloader(IPreloadEvents* pPreloader = nullptr);
		virtual ~ResourcePreloader();

		void LoadCache();
		void SaveCache();

	protected:
		std::vector<AssetFileFilter*> m_filters;
	};
}
