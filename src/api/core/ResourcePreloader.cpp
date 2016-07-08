#include "../stdafx.h"

#include "ResourcePreloader.h"
#include "../utils/ResourceBuilder.h"

using FWdebug::Exception;
using namespace FWdebugExceptions;

using namespace Grafkit;

#include "../generator/TextureLoader.h"

// egyelore csak a texturakat cacheljuk be elore
namespace {
	enum {
		TEXTURES
	};
	struct {
		int type;
		const char *path;
		const char *extensions[8];
	} preloadFilters[] = {
		{ TEXTURES, "./textures/", {"jpg", "png", "tga", "gif", "dds", "bmp", nullptr, nullptr, }, },
	};
}

AssetPreloader::AssetPreloader(PreloadEvents * pPreloader) : Grafkit::IResourceManager()
{
	ZeroMemory(&m_filters, sizeof(m_filters));
	for (size_t i = 0; i < sizeof(preloadFilters) / sizeof(preloadFilters[0]); i++) {
		m_filters.push_back(new AssetFileFilter(preloadFilters[i].extensions, 8));
	}
}

AssetPreloader::~AssetPreloader()
{
}

// ============================================================================================================

void Grafkit::AssetPreloader::LoadCache()
{
	for (size_t i = 0; i < m_filters.size(); i++) 
	{
		if (m_filters[i] == nullptr)
			continue;

			IAssetFactory::filelist_t filelist = this->GetAssetFactory()->GetAssetList(m_filters[i]);

			if (!filelist.empty()) for (IAssetFactory::filelist_t::iterator it = filelist.begin(); it != filelist.end(); it++)
			{

				std::string filename = *it, name, path, ext;
				AssetFileFilter::trimpath(filename, path, name, ext);

				int type = preloadFilters[i].type;

				switch (type) 
				{
				case TEXTURES:
				{
					Reload(new TextureFromBitmap(name, filename));
				}
				break;

				}
			}
		}

	/// --- load file cache if any

}

void Grafkit::AssetPreloader::SaveCache()
{
	/// -- save cached files if any 
}

// ============================================================================================================
