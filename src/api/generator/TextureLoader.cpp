#include "TextureLoader.h"



TextureLoader::TextureLoader()
{
}

Grafkit::TextureFromBitmap::TextureFromBitmap(std::string name, std::string source_name) : ITextureBuilder(name, source_name),
	m_w(0), m_h(0)
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
	TextureRef texture = new Texture();

	LOGGER(log(info) << "loading texture from resource");
	
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
