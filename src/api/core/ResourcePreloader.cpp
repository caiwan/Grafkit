#include "ResourcePreloader.h"

#include "ResourceBuilder.h"

#include "easyloggingpp.h"
#include "exceptions.h"

using FWdebug::Exception;
using namespace FWdebugExceptions;

using namespace Grafkit;

/// ---- ezt lehet kulon fileba kene tenni
#include "../render/texture.h"
#include "../generator/TextureLoader.h"

#include "../render/text.h"
#include "../generator/FontmapLoader.h"

///@todo a shader betoltes legyen on-the-fly, es ne itt
#include "../render/shader.h"
#include "../generator/ShaderLoader.h"


///@todo kipucolni a kodot

namespace {

	enum {
		TEXTURES,
		FONT,
		//SHADERS
	};

	struct {
		//IResource::RA_type_e type;
		int type;
		const char *bucket_names;
		const char *basedir;
		const char *extensions[8];
	} rules[] = {
		{ TEXTURES, TEXTURE_BUCKET, "./textures/", {"jpg", "png", "tga", "gif", nullptr, nullptr, nullptr, nullptr, }, },
		/* {FONT, FONT_BUCKET "./fonts/", {"bmf", "bmt", "bmx", nullptr, nullptr, nullptr, nullptr, nullptr, }, },*/ 
		
		///@todo a textura betoltes on-the-fly lesz
		//{ SHADERS, SHADER_BUCKET , "./shaders/",{ "hlsl", "fx", "vs", "fs", "gm", "cp", "ps", nullptr, }, },
	};

	/*const char * shader_postfix[] = {
	"", "vertex", "pixel", "gemoetry", "compute"
	};*/
}

AssetPreloader::AssetPreloader(PreloadEvents * pPreloader) : Grafkit::IResourceManager(), m_pPreloader(nullptr)
{
	ZeroMemory(&m_filters, sizeof(m_filters));
	for (size_t i = 0; i < sizeof(rules) / sizeof(rules[0]); i++) {
		m_filters.push_back(new AssetFileFilter(rules[i].extensions, 8));
	}
}


AssetPreloader::~AssetPreloader()
{
	/// @todo ez itt baszik valamit eppen - double delete?
	/*
	for (size_t i = 0; i < IResource::RA_TYPE_COUNT; i++) {
		//delete m_filters[rules[i].type];
	}
	*/
}

// ============================================================================================================

void Grafkit::AssetPreloader::LoadCache()
{
	/// --- itt becacheli az osszes filet, mait lehet

	//IRenderAssetRepository* repo = this->GetRepository(ROOT_REPOSITORY);

	for (size_t i = 0; i < m_filters.size(); i++) 
	{
		if (m_filters[i] == nullptr)
			continue;

		for (size_t j = 0; j < this->m_loaders.size(); j++) {
			IAssetFactory *loader = this->m_loaders[j];
			IAssetFactory::filelist_t filelist = loader->GetAssetList(m_filters[i]);

			if (!filelist.empty()) for (IAssetFactory::filelist_t::iterator it = filelist.begin(); it != filelist.end(); it++)
			{

				std::string filename = *it, name, path, ext;
				AssetFileFilter::trimpath(filename, path, name, ext);

				/*IResource::RA_type_e type = (IResource::RA_type_e) i;*/
				int type = rules[i].type;

				switch (type) 
				{
				case TEXTURES:
				{
					///@tod folytkov

					//TextureAsset* texture = new TextureAsset;
					//texture->SetName(name);

					//// get the pointer right from the asset container, and feed it 
					//
					//TextureAssetRef txptr = dynamic_cast<TextureAsset*>(repo->GetObjPtrByGlobalID(repo->AddObject(texture)).Get());
					//m_builders.push_back(new TextureFromBitmap(loader->Get(filename), txptr));
					
				}
				break;

				case FONT:
				{}
				break;

				///@todo a shader betoltes legyen inkabb on-the-fly
				///@todo: honnan tudjuk, hogy milyen shadert, es honnan generaltunk le?
# if 0
				case SHADERS:		/** Vegigprobalunk mindenfele shader tipust*/
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
						ShaderAssetRef shPtr = dynamic_cast<ShaderAsset*>(repo->GetObjPtrByGlobalID(repo->AddObject(shader)).Get());
						m_builders.push_back(new ShaderLoader(loader->GetResourceByName(filename), sh_typ, shPtr));
					}
				}
#endif
				break;
				}
			}
		}
	}

	/// --- load cache if any

}

void Grafkit::AssetPreloader::SaveCache()
{
	/// -- save cached files if any 
}

// ============================================================================================================

IAssetRef Grafkit::AssetPreloader::GetResource(std::string filename)
{
	for (size_t i = 0; i < this->m_loaders.size(); i++) {
		try {
			return this->m_loaders[i]->Get(filename);
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

void Grafkit::AssetPreloader::DoPrecalc()
{
	if (m_pPreloader) m_pPreloader->OnBeginLoad();
	
	size_t len = m_builders.size(), i = 0;

	for (std::list<IResourceBuilder*>::iterator it = m_builders.begin(); it != m_builders.end(); it++)
	{
		LOG(INFO) << "Loading asset" << (int)i << "of" << (int)len;

		(**it)(this);
		
		if (m_pPreloader) m_pPreloader->OnElemLoad(i, len);
		i++;
	}

	if (m_pPreloader) m_pPreloader->OnEndLoad();
}
