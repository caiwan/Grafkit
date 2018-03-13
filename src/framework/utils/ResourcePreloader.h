#pragma once

#include "../utils/exceptions.h"

#include <vector>
#include <string>

#include "ResourceManager.h"
#include "AssetFactory.h"

#include "../render/renderer.h"


namespace Grafkit{
	/**
	Ez egy olyan dolog, ami automatikusan betolt, es cachel resourcokat
	Dirty hack of mine
	Ide fogom behackelni a livereload managert is 
	*/
	class ResourcePreloader : public Grafkit::IResourceManager
	{
	public:
		ResourcePreloader(IPreloadEvents* pPreloader = nullptr);
		virtual ~ResourcePreloader();

		void LoadCache();
		void SaveCache();

	protected:
		std::vector<AssetFileFilter*> m_filters;
	};
}
