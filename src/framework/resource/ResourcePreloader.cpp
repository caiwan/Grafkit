#include "stdafx.h"

#include "ResourcePreloader.h"
#include "loaders/TextureLoader.h"
#include "utils/asset/AssetFactory.h"
#include "render/Texture.h"

using FWdebug::Exception;
using namespace FWdebugExceptions;

using namespace Grafkit;

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

// TODO: Lagacy stuff, get rid of it
ResourcePreloader::ResourcePreloader(IPreloadEvents * pPreloader) : IResourceManager()
{
	ZeroMemory(&m_filters, sizeof(m_filters));
	for (size_t i = 0; i < sizeof(preloadFilters) / sizeof(preloadFilters[0]); i++) {
		m_filters.push_back(new AssetFileFilter(preloadFilters[i].extensions, 8));
	}
	SetPreloadListener(pPreloader);
}

// TODO: Lagacy stuff, get rid of it
ResourcePreloader::~ResourcePreloader()
{
	for (size_t i = 0; i < m_filters.size(); i++) {
		delete m_filters[i];
	}
}

// ============================================================================================================

// TODO: Lagacy stuff, get rid of it
void ResourcePreloader::LoadCache()
{
	for (size_t i = 0; i < m_filters.size(); i++) 
	{
		if (m_filters[i] == nullptr)
			continue;

			IAssetFactory::filelist_t filelist = this->GetAssetFactory()->GetAssetList(m_filters[i]);

			if (!filelist.empty()) for (IAssetFactory::filelist_t::iterator it = filelist.begin(); it != filelist.end(); ++it)
			{

				std::string filename = *it, name, path, ext;
				AssetFileFilter::TrimPath(filename, path, name, ext);

				int type = preloadFilters[i].type;

				switch (type) 
				{
				case TEXTURES:
				{
                    // TODO: Lagacy stuff, get rid of it
                    std::string uuid = "";
                    TextureBitmapParams params {filename};
					Reload(new TextureFromBitmap(name, uuid, params));
                    // assert(0); 
				}
				break;
				}
			}
		}

	/// --- load file cache if any

}

void ResourcePreloader::SaveCache()
{
	/// -- save cached files if any 
    // name:uuid:path
}

// ============================================================================================================

ParametricResourceLoader::ParametricResourceLoader(IPreloadEvents* pPreloader) : IResourceManager(pPreloader){

}

ParametricResourceLoader::~ParametricResourceLoader() {
}

//void ParametricResourceLoader::RegisterBuilderFactory(IResourceBuilderFactroy* factroy) {
//}

void ParametricResourceLoader::LoadCache() {
}

void ParametricResourceLoader::SaveCache() {
}

//void ParametricResourceLoader::PushNewBuilder(std::string builder, std::string name, std::string uuid, void* const& params) {
//}

