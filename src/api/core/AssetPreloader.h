#pragma once

#include <vector>
#include <string>

#include "renderassets.h"
#include "assets.h"

namespace FWassets{
	/**
	Ez egy olyan dolog, ami automatikusan betolt, es cachel objektumokat
	*/
	class AssetPreloader : public FWassets::IRenderAssetManager
	{
	public:
		AssetPreloader();
		~AssetPreloader();

		//FWassets::IResourceFactory* GetResourceFactory();
		virtual FWrender::Renderer & GetDeviceContext() = 0;

		void RegisterRecourceFactory(IResourceFactory* factory) { m_factories.push_back(factory); }

		void LoadCache();
		void SaveCache();

		std::list<IRenderAssetBuilder*> GetBuilders() { return m_generated_builders; }

		IRenderAsset* GetResource(std::string filename);

	protected:
		std::vector<IResourceFactory*> m_factories;
		ResourceFilter* m_filters[IRenderAsset::RA_TYPE_COUNT];
		std::list<IRenderAssetBuilder*> m_generated_builders;
	};
}
