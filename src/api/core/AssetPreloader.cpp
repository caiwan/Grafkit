#include "AssetPreloader.h"

using namespace FWassets;

namespace {
	struct {
		IRenderAsset::RA_type_e type;
		const char *extensions[8];
	} rules[] = {
		{ FWassets::IRenderAsset::RA_TYPE_Texture, {"jpg", "png", "tga", "gif", nullptr, nullptr, nullptr, nullptr, }, },
		{ FWassets::IRenderAsset::RA_TYPE_Font, {"bmf", "bmt", "bmx", nullptr, nullptr, nullptr, nullptr, nullptr, }, },
	};
}

AssetPreloader::AssetPreloader() : FWassets::IRenderAssetManager()
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

/// ---- ezt lehet kulon fileba kene tenni
#include "../render/texture.h"
#include "../render/text.h"

using FWrender::Texture;
using FWrender::TextureFromBitmap;
using FWrender::TextureRef;

void FWassets::AssetPreloader::LoadCache()
{
	/// --- itt becacheli az osszes filet, mait lehet
	for (size_t i = 0; i < IRenderAsset::RA_TYPE_COUNT; i++) 
	{
		if (m_filters[i] == nullptr)
			continue;

		filelist_t filelist;
		for (size_t j = 0; j < this->m_factories.size(); j++) {
			filelist_t ret_filelist = this->m_factories[j]->GetResourceList(m_filters[i]);
			filelist.insert(filelist.end(), ret_filelist.begin(), ret_filelist.end());
		}

		for (filelist_t::iterator it = filelist.begin(); it != filelist.end(); it++) 
		{

			// + trim path goez here 
			std::string filename = *it;

			switch (i) {
			case FWassets::IRenderAsset::RA_TYPE_Texture :
			{
				TextureRef texture = new Texture;
				texture->SetName(filename);

				/// ennek itten szamtalan hibaja van kerem
				// TextureFromBitmap* generator = new TextureFromBitmap(FWassets::IResourceRef(), texture);

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

void FWassets::AssetPreloader::SaveCache()
{
}


// folytkov ... 
