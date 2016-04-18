#include "TextureLoader.h"



TextureLoader::TextureLoader()
{
}


TextureLoader::~TextureLoader()
{
	TextureResRef outTexture = (TextureResRef_t)m_dstResource;

	if (outTexture.Invalid()) {
		outTexture = new TextureRes;
	}

	// ha van textura, akkor torolje le 
	if (outTexture->Valid()) {
		outTexture->Release();
	}

	LOGGER(LOG(INFO) << "Loading texture from resource");

	int x = 0, y = 0, ch = 0;
	/// @todo resize;

	IAssetRef asset = this->GetSourceAsset(assman);

	// kikenyszeritett RGBA mod
	UCHAR *data = stbi_load_from_memory((UCHAR *)asset->GetData(), asset->GetSize(), &x, &y, &ch, 0);

	if (!data)
	{
		throw EX_DETAILS(BitmapLoadException, stbi_failure_reason());
	}

	TextureRef texture = new Texture();
	if (texture.Invalid())
		texture = new Texture();

	try {
		texture->Initialize(assman->GetDeviceContext(), new Bitmap(data, x, y, ch));
	}
	catch (FWdebug::Exception &e) {
		// do nothing here yet.
		throw e;
	}

	outTexture = texture;
	m_dstResource = outTexture;
}
