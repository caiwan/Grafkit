#pragma once

#include "../utils/exceptions.h"

#include <vector>
#include <string>

#include "../utils/ResourceManager.h"
#include "../utils/AssetFactory.h"

#include "../render/renderer.h"


namespace Grafkit{
	/**
	Ez egy olyan dolog, ami automatikusan betolt, es cachel resourcokat
	Dirty hack of mine
	Ide fogom behackelni a livereload managert is 
	*/
	class AssetPreloader : public Grafkit::IResourceManager
	{
	public:
		AssetPreloader(PreloadEvents* pPreloader = nullptr);
		~AssetPreloader();

		void LoadCache();
		void SaveCache();

	protected:
		std::vector<AssetFileFilter*> m_filters;
	};
}
