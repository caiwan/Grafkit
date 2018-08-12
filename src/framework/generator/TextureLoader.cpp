#include "stdafx.h"

#include "TextureLoader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "render/Texture.h"
#include "utils/AssetFactory.h"
#undef STB_IMAGE_IMPLEMENTATION

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#undef STB_IMAGE_RESIZE_IMPLEMENTATION


using namespace Grafkit;
using namespace FWdebugExceptions;


// ========================================================================================================================
// Texture buffer generator
// ========================================================================================================================

IResource * ITexture1DBuilder::NewResource()
{
    return new Texture1DRes;
}

IResource * ITexture2DBuilder::NewResource()
{
    return new Texture2DRes;
}

// ========================================================================================================================
// Texture buffer generator
// ========================================================================================================================

TextureBufferBuilder::TextureBufferBuilder(std::string name, TextureTypeE type) : ITexture2DBuilder(name),
m_w(0), m_h(0), m_type(type)
{
}

TextureBufferBuilder::TextureBufferBuilder(std::string name, TextureTypeE type, uint16_t w, uint16_t h) : ITexture2DBuilder(name),
m_w(w), m_h(h), m_type(type)
{
}

void TextureBufferBuilder::Load(IResourceManager * const & resman, IResource * source)
{
    TextureResRef dstTexture = dynamic_cast<Texture2DRes*>(source);
    if (dstTexture.Invalid()) {
        THROW_EX(NullPointerException);
    }

    TextureRef texture = new Texture2D();

    switch (m_type) {
    case TB_RGBA:
        texture->Initialize(resman->GetDeviceContext(), m_w, m_h);
        break;
    case TB_RGBA32:
        texture->InitializeFloatRGBA(resman->GetDeviceContext(), m_w, m_h);
        break;
    case TB_Float:
        texture->InitializeFloat(resman->GetDeviceContext(), m_w, m_h);
        break;
    case TB_Depth:
        texture->InitializeDepth(resman->GetDeviceContext(), m_w, m_h);
        break;
    }

    if (dstTexture->Valid()) {
        dstTexture->Release();
    }

    dstTexture->AssingnRef(texture);
}


// ========================================================================================================================
// Texture from bitmap loader
// ========================================================================================================================

TextureFromBitmap::TextureFromBitmap(std::string name, std::string sourceName, std::string uuid) : ITexture2DBuilder(name, sourceName, uuid), m_w(0), m_h(0)
{
}

TextureFromBitmap::~TextureFromBitmap()
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

void TextureFromBitmap::Load(IResourceManager * const & resman, IResource * source)
{
    TextureResRef dstTexture = dynamic_cast<Texture2DRes*>(source);
    if (dstTexture.Invalid()) {
        THROW_EX(NullPointerException);
    }

    // --- load texture 
    TextureRef texture = new Texture2D();

    LOGGER(Log::Logger().Trace("loading texture from resource"));

    int x = 0, y = 0, ch = 0;
    IAssetRef asset = this->GetSourceAsset(resman);

    // kikenyszeritett rgba mod
    UCHAR *data = stbi_load_from_memory(static_cast<UCHAR *>(asset->GetData()), asset->GetSize(), &x, &y, &ch, 0);

    /// @todo resize;

    if (!data)
    {
        THROW_EX_DETAILS(BitmapLoadException, stbi_failure_reason());
    }

    texture->Initialize(resman->GetDeviceContext(), new Bitmap(data, x, y, ch));

    // --- xchg texture 
    if (dstTexture->Valid()) {
        dstTexture->Release();
    }

    dstTexture->AssingnRef(texture);
}

/*
*/

TextureCubemapFromBitmap::TextureCubemapFromBitmap(std::string name,
    std::string source_posx,
    std::string source_negx,
    std::string source_posy,
    std::string source_negy,
    std::string source_posz,
    std::string source_negz) : ITexture2DBuilder(name, "")
{
    m_sourceNames[0] = source_posx;
    m_sourceNames[1] = source_negx;
    m_sourceNames[2] = source_posy;
    m_sourceNames[3] = source_negy;
    m_sourceNames[4] = source_posz;
    m_sourceNames[5] = source_negz;
}

TextureCubemapFromBitmap::~TextureCubemapFromBitmap()
{
}

void TextureCubemapFromBitmap::Load(IResourceManager * const & resman, IResource * source)
{
    TextureCubeResRef dstTexture = dynamic_cast<TextureCubeRes*>(source);
    if (dstTexture.Invalid()) {
        THROW_EX(NullPointerException);
    }

    BitmapRef bitmaps[6];

    for (int i = 0; i < 6; i++) {
        int x = 0, y = 0, ch = 0;
        IAssetRef asset = resman->GetAssetFactory()->Get(m_sourceNames[i]);

        // kikenyszeritett rgba mod
        uint8_t *data = stbi_load_from_memory(static_cast<uint8_t*>(asset->GetData()), asset->GetSize(), &x, &y, &ch, 0);

        /// @todo resize;

        if (!data)
        {
            THROW_EX_DETAILS(BitmapLoadException, stbi_failure_reason());
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

IResource * TextureCubemapFromBitmap::NewResource()
{
    return new TextureCubeRes();
}

// -------------------------------------------------------------------------------

TextureNoiseMap::TextureNoiseMap(size_t size) : TextureFromBitmap("NOISE"), m_size(size)
{
}

TextureNoiseMap::TextureNoiseMap(std::string name, size_t size) : TextureFromBitmap(name), m_size(size)
{
}

void TextureNoiseMap::Load(IResourceManager * const & resman, IResource * source)
{
    TextureResRef dstTexture = dynamic_cast<Texture2DRes*>(source);
    if (dstTexture.Invalid()) {
        THROW_EX(NullPointerException);
    }

    // --- load texture 
    TextureRef texture = new Texture2D();

    LOGGER(Log::Logger().Trace("loading texture from resource"));

    size_t k = m_size * m_size * 4;
    UCHAR *data = new UCHAR[m_size * m_size * 4];

    for (int i = 0; i < k; i++) {
        data[i] = rand() % 255;
    }

    texture->Initialize(resman->GetDeviceContext(), new Bitmap(data, m_size, m_size, 4));

    // --- xchg texture 
    if (dstTexture->Valid()) {
        dstTexture->Release();
    }

    dstTexture->AssingnRef(texture);
}

TextureSamplerBuilder::TextureSamplerBuilder(Type_E type) : IResourceBuilder("", "")
{
    switch (type)
    {
    case TGG_Clamping:
        m_name = TS_NAME_CLAMP;
        m_mode = D3D11_TEXTURE_ADDRESS_CLAMP;
        break;
    default:
        m_name = TS_NAME_WRAP;
        m_mode = D3D11_TEXTURE_ADDRESS_WRAP;
        break;
    }
}

void TextureSamplerBuilder::Load(IResourceManager * const & resman, IResource * source)
{
    TextureSamplerResRef sampler = dynamic_cast<Resource<TextureSampler>*>(source);

    if (sampler.Invalid()) {
        THROW_EX(NullPointerException);
    }

    sampler->AssingnRef(new TextureSampler());
    sampler->Get()->Initialize(resman->GetDeviceContext(), m_mode);
}

IResource * TextureSamplerBuilder::NewResource()
{
    return new Resource<TextureSampler>();
}
