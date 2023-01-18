#include "AssetPreloader.h"

#include "easyloggingpp.h"
#include "exceptions.h"

using FWdebug::Exception;
using namespace FWdebugExceptions;

using namespace FWassets;

namespace {
	struct {
		IRenderAsset::RA_type_e type;
		const char *basedir;
		const char *extensions[8];
	} rules[] = {
		{ FWassets::IRenderAsset::RA_TYPE_Texture, "./", {"jpg", "png", "tga", "gif", nullptr, nullptr, nullptr, nullptr, }, },
		{ FWassets::IRenderAsset::RA_TYPE_Font, "./", {"bmf", "bmt", "bmx", nullptr, nullptr, nullptr, nullptr, nullptr, }, },
	};
}

AssetPreloader::AssetPreloader(PreloadEvents * pPreloader) : FWassets::IRenderAssetManager(), m_pPreloader(nullptr)
{
	ZeroMemory(&m_filters, sizeof(m_filters));
	for (size_t i = 0; i < sizeof(rules) / sizeof(rules[0]); i++) {
		m_filters[rules[i].type] = new ResourceFilter(rules[i].extensions, 8);
	}
}


AssetPreloader::~AssetPreloader()
{
	for (size_t i = 0; i < IRenderAsset::RA_TYPE_COUNT; i++) {
		delete m_filters[rules[i].type];
	}
}

// ============================================================================================================
/// ---- ezt lehet kulon fileba kene tenni
#include "../render/texture.h"
#include "../render/text.h"

using FWrender::TextureFromBitmap;
using FWrender::TextureAsset;

void FWassets::AssetPreloader::LoadCache()
{
	/// --- itt becacheli az osszes filet, mait lehet
	for (size_t i = 0; i < IRenderAsset::RA_TYPE_COUNT; i++) 
	{
		if (m_filters[i] == nullptr)
			continue;

		for (size_t j = 0; j < this->m_loaders.size(); j++) {
			IResourceFactory *loader = this->m_loaders[j];
			filelist_t filelist = loader->GetResourceList(m_filters[i]);

			if (!filelist.empty()) for (filelist_t::iterator it = filelist.begin(); it != filelist.end(); it++)
			{

				std::string filename = *it, name, path, ext;
				ResourceFilter::trimpath(filename, path, name, ext);

				IRenderAsset::RA_type_e type = (IRenderAsset::RA_type_e) i;

				switch (type) 
				{
				case FWassets::IRenderAsset::RA_TYPE_Texture:
				{
					TextureAsset* texture = new TextureAsset;
					texture->SetName(name);

					m_builders.push_back(new TextureFromBitmap(loader->GetResourceByName(filename), texture));
					AddObject(texture);
				}
				break;

				case FWassets::IRenderAsset::RA_TYPE_Font:
				{}
				break;

				// a tobbiekkel egyelore nem foglalkozunk; ide jonnek majd azok is
				//case FWassets::IRenderAsset::RA_TYPE_Material:
				//{}
				//break;

				//case FWassets::IRenderAsset::RA_TYPE_Shader:
				//{}
				//break;
				}
			}
		}
	}

	/// --- load cache if any

}

void FWassets::AssetPreloader::SaveCache()
{
	/// -- save cached files if any 
}

// ============================================================================================================

IResourceRef FWassets::AssetPreloader::GetResource(std::string filename)
{
	for (size_t i = 0; i < this->m_loaders.size(); i++) {
		try {
			return this->m_loaders[i]->GetResourceByName(filename);
		}
		catch (FileNotFoundException &e)
		{
			LOG(INFO) << "Could not found file in generator " << i << filename << "moving on";
			continue;
		}
		catch (Exception &e) {
			LOG(WARNING) << "Other error type of exception was thrown";
			throw e;
		}
	}

	throw EX_DETAILS(FileNotFoundException, filename.c_str());
}

void FWassets::AssetPreloader::DoPrecalc()
{
	if (m_pPreloader) m_pPreloader->OnBeginLoad();
	
	size_t len = m_builders.size(), i = 0;

	for (std::list<IRenderAssetBuilder*>::iterator it = m_builders.begin(); it != m_builders.end(); it++)
	{
		(**it)(this);
		if (m_pPreloader) m_pPreloader->OnElemLoad(i, len);
		i++;
	}

	if (m_pPreloader) m_pPreloader->OnEndLoad();
}
