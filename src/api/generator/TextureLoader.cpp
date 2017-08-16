#include "stdafx.h"

#include "TextureLoader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#undef STB_IMAGE_IMPLEMENTATION

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize.h"
#undef STB_IMAGE_RESIZE_IMPLEMENTATION


using namespace Grafkit;
using namespace FWdebugExceptions;


// ========================================================================================================================
// Texture from bitmap loader
// ========================================================================================================================
//Grafkit::TextureFromBitmap::TextureFromBitmap(std::string source_name) : ITextureBuilder(source_name, source_name),
//	m_w(0), m_h(0)
//{
//}

Grafkit::TextureFromBitmap::TextureFromBitmap(std::string name, std::string source_name) : ITextureBuilder(name, source_name), m_w(0), m_h(0)
{
}

Grafkit::TextureFromBitmap::~TextureFromBitmap()
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

void Grafkit::TextureFromBitmap::Load(Grafkit::IResourceManager * const & resman, Grafkit::IResource * source)
{
	TextureResRef dstTexture = dynamic_cast<TextureRes*>(source);
	if (dstTexture.Invalid()) {
		throw EX(NullPointerException);
	}

	// --- load texture 
	TextureRef texture = new Texture2D();

	LOGGER(Log::Logger().Trace("loading texture from resource"));

	int x = 0, y = 0, ch = 0;
	IAssetRef asset = this->GetSourceAsset(resman);

	// kikenyszeritett rgba mod
	UCHAR *data = stbi_load_from_memory((UCHAR *)asset->GetData(), asset->GetSize(), &x, &y, &ch, 0);

	/// @todo resize;

	if (!data)
	{
		throw EX_DETAILS(BitmapLoadException, stbi_failure_reason());
	}

	texture->Initialize(resman->GetDeviceContext(), new Bitmap(data, x, y, ch));

	// --- xchg texture 
	if (dstTexture->Valid()) {
		dstTexture->Release();
	}

	dstTexture->AssingnRef(texture);
}

IResource * Grafkit::ITextureBuilder::NewResource()
{
	return new TextureRes;
}

/*
*/

Grafkit::TextureCubemapFromBitmap::TextureCubemapFromBitmap(std::string name, std::string source_posx, std::string source_negx, std::string source_posy, std::string source_negy, std::string source_posz, std::string source_negz) : ITextureBuilder(name, "")
{
	m_sourceNames[0] = source_negx;
	m_sourceNames[1] = source_negy;
	m_sourceNames[2] = source_negz;
	m_sourceNames[3] = source_posx;
	m_sourceNames[4] = source_posy;
	m_sourceNames[5] = source_posz;
}

Grafkit::TextureCubemapFromBitmap::~TextureCubemapFromBitmap()
{
}

void Grafkit::TextureCubemapFromBitmap::Load(Grafkit::IResourceManager * const & resman, Grafkit::IResource * source)
{
	TextureCubeResRef dstTexture = dynamic_cast<TextureCubeRes*>(source);
	if (dstTexture.Invalid()) {
		throw EX(NullPointerException);
	}

	BitmapRef bitmaps[6];

	for (int i = 0; i < 6; i++) {
		int x = 0, y = 0, ch = 0;
		IAssetRef asset = this->GetSourceAsset(resman);

		// kikenyszeritett rgba mod
		UCHAR *data = stbi_load_from_memory((UCHAR *)asset->GetData(), asset->GetSize(), &x, &y, &ch, 0);

		/// @todo resize;

		if (!data)
		{
			throw EX_DETAILS(BitmapLoadException, stbi_failure_reason());
		}

		bitmaps[i] = new Bitmap(data, x, y, ch);
	}

	TextureCubeRef texture = new TextureCube();
	texture->Initialize(resman->GetDeviceContext(), new Cubemap(bitmaps));

	// --- xchg texture 
	if (dstTexture->Valid()) {
		dstTexture->Release();
	}

	dstTexture->AssingnRef(texture);
}

IResource * Grafkit::TextureCubemapFromBitmap::NewResource()
{
	return new TextureCubeRes();
}
