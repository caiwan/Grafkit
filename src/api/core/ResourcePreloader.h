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
	*/
	class AssetPreloader : public Grafkit::IResourceManager
	{
	public:
		AssetPreloader(PreloadEvents* pPreloader = nullptr);
		~AssetPreloader();

		//Grafkit::IAssetFactory* GetResourceFactory();
		virtual Grafkit::Renderer & GetDeviceContext() = 0;

		void LoadCache();
		void SaveCache();

	protected:
		std::vector<AssetFileFilter*> m_filters;
	};
}
