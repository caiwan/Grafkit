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
		{ FWassets::IRenderAsset::RA_TYPE_Texture, "./textures/", {"jpg", "png", "tga", "gif", nullptr, nullptr, nullptr, nullptr, }, },
		{ FWassets::IRenderAsset::RA_TYPE_Font, "./fonts/", {"bmf", "bmt", "bmx", nullptr, nullptr, nullptr, nullptr, nullptr, }, },
		{ FWassets::IRenderAsset::RA_TYPE_Shader, "./shaders/",{ "hlsl", "fx", "vs", "fs", "gm", "cp", "ps", nullptr, }, },
		//{ FWassets::IRenderAsset::RA_TYPE_Shader_VS, "./shaders/",{ "hlsl", "fx", "vs", nullptr, nullptr, nullptr, nullptr, nullptr, }, },
		//{ FWassets::IRenderAsset::RA_TYPE_Shader_PS, "./shaders/",{ "hlsl", "fx", nullptr, "fs", "ps", nullptr, nullptr, nullptr, }, },
		//{ FWassets::IRenderAsset::RA_TYPE_Shader_GM, "./shaders/",{ "hlsl", "fx", nullptr, nullptr, "gm", nullptr, nullptr, nullptr, }, },
		//{ FWassets::IRenderAsset::RA_TYPE_Shader_CP, "./shaders/",{ "hlsl", "fx", nullptr, nullptr, "cp", nullptr, nullptr, }, },
	};

	const char * shader_postfix[] = {
	"", "vertex", "pixel", "gemoetry", "compute"
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
	/// @todo ez itt baszik valamit eppen - double delete?
	for (size_t i = 0; i < IRenderAsset::RA_TYPE_COUNT; i++) {
		//delete m_filters[rules[i].type];
	}
}

// ============================================================================================================
/// ---- ezt lehet kulon fileba kene tenni
#include "../render/texture.h"
#include "../render/text.h"

using FWrender::TextureFromBitmap;
using FWrender::TextureAssetRef;
using FWrender::TextureAsset;

#include "../render/shader.h"
using FWrender::ShaderLoader;
using FWrender::ShaderAssetRef;
using FWrender::ShaderAsset;
using FWrender::ShaderType_e;

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

					// get the pointer right from the asset container, and feed it 
					TextureAssetRef txptr = dynamic_cast<TextureAsset*>(m_assets[AddObject(texture)].Get()); 
					m_builders.push_back(new TextureFromBitmap(loader->GetResourceByName(filename), txptr));
					
				}
				break;

				case FWassets::IRenderAsset::RA_TYPE_Font:
				{}
				break;

				// a tobbiekkel egyelore nem foglalkozunk; ide jonnek majd azok is
				//case FWassets::IRenderAsset::RA_TYPE_Material:
				//{}
				//break;

				///@todo: honnan tudjuk, hogy milyen shadert, es honnan generaltunk le?
				case IRenderAsset::RA_TYPE_Shader:		/** Vegigprobalunk mindenfele shader tipust*/
				{
					///@todo eloforditott shaderekkel is tudjon kezdeni valamit

					//for (ShaderType_e sh_typ = ShaderType_e::ST_NONE; sh_typ < ShaderType_e::ST_COUNT; sh_typ++) 
					for (size_t j = 0; j < ShaderType_e::ST_COUNT; j++)
					{
						ShaderType_e sh_typ = (ShaderType_e)j;
						if (sh_typ == FWrender::ST_NONE) ///@todo ebbol lenne az includes
							continue;

						///@todo honnan a bubajbol fogjuk mi azt tudni, hogy eppen milyen shaderek vannak? - ezeket lehet, hogy nem igy kellene csinalni
						ShaderAsset* shader = new ShaderAsset;
						if (sh_typ != FWrender::ST_NONE) 
							shader->SetName(name + ":" + shader_postfix[sh_typ]);
						else 
							shader->SetName(name);

						// get the pointer right from the asset container, and feed it 
						ShaderAssetRef shPtr = dynamic_cast<ShaderAsset*>(m_assets[AddObject(shader)].Get());
						m_builders.push_back(new ShaderLoader(loader->GetResourceByName(filename), sh_typ, shPtr));
					}
				}

				break;
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
		LOG(INFO) << "Loading asset" << (int)i << "of" << (int)len;

		(**it)(this);
		
		if (m_pPreloader) m_pPreloader->OnElemLoad(i, len);
		i++;
	}

	if (m_pPreloader) m_pPreloader->OnEndLoad();
}
