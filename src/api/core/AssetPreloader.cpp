#include "AssetPreloader.h"

using namespace FWassets;

namespace {
	struct {
		IRenderAsset::RA_type_e type;
		const char *extensions[];
	} rules[] = {
		{RA_TYPE_Texture, {"jpg", "png", "tga", "gif"},},
		{RA_TYPE_Font, {"bmf", "bmt", "bmx"},},
	};
	//RA_TYPE_Texture,
	//RA_TYPE_Font,
	//RA_TYPE_Material,
	//RA_TYPE_Shader,
	//RA_TYPE_COUNT
}
AssetPreloader::AssetPreloader()
{
}


AssetPreloader::~AssetPreloader()
{
}


// folytkov ... 
