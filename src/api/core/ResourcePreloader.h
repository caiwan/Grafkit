#pragma once

#include "exceptions.h"

#include <vector>
#include <string>

#include "ResourceManager.h"
#include "AssetFactory.h"

#include "../render/renderer.h"


namespace Grafkit{

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
	class AssetPreloader : public Grafkit::IResourceManager
	{
	public:
		//AssetPreloader();
		AssetPreloader(PreloadEvents* pPreloader = nullptr);
		~AssetPreloader();

		//Grafkit::IAssetFactory* GetResourceFactory();
		virtual Grafkit::Renderer & GetDeviceContext() = 0;

		void RegisterRecourceFactory(IAssetFactory* factory) { m_loaders.push_back(factory); }

		void LoadCache();
		void SaveCache();

		void AddBuilder(IResourceBuilder* ptr) { m_builders.push_back(ptr); }
		void AddBuilders() {}

		std::list<IResourceBuilder*> GetBuilders() { return m_builders; }
		IAssetRef GetResource(std::string filename);

		void DoPrecalc();

	protected:
		std::vector<IAssetFactory*> m_loaders;
		
		///@todo folytkov
		std::vector<AssetFileFilter*> m_filters; // [IResource::RA_TYPE_COUNT];
		std::list<IResourceBuilder*> m_builders;

		PreloadEvents* m_pPreloader;
	};
}
