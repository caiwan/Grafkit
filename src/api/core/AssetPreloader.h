#pragma once

#include "exceptions.h"

#include <vector>
#include <string>

#include "renderassets.h"
#include "assets.h"

namespace FWassets{

	/// Ez lesz majd a betoltocsik
	class PreloadEvents {
	public:
		virtual void OnBeginLoad() = 0;
		virtual void OnElemLoad(size_t actual, size_t count) = 0;
		virtual void OnEndLoad() = 0;
	};

	/**
	Ez egy olyan dolog, ami automatikusan betolt, es cachel objektumokat
	*/
	class AssetPreloader : public FWassets::IRenderAssetManager
	{
	public:
		//AssetPreloader();
		AssetPreloader(PreloadEvents* pPreloader = nullptr);
		~AssetPreloader();

		//FWassets::IResourceFactory* GetResourceFactory();
		virtual FWrender::Renderer & GetDeviceContext() = 0;

		void RegisterRecourceFactory(IResourceFactory* factory) { m_loaders.push_back(factory); }

		void LoadCache();
		void SaveCache();

		void AddBuilder(IRenderAssetBuilder* ptr) { m_builders.push_back(ptr); }
		void AddBuilders() {}

		std::list<IRenderAssetBuilder*> GetBuilders() { return m_builders; }
		IResourceRef GetResource(std::string filename);

		void DoPrecalc();

	protected:
		std::vector<IResourceFactory*> m_loaders;
		ResourceFilter* m_filters[IRenderAsset::RA_TYPE_COUNT];
		std::list<IRenderAssetBuilder*> m_builders;

		PreloadEvents* m_pPreloader;
	};
}
