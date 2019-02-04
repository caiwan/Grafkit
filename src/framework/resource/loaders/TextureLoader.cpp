#include "stdafx.h"

#include "TextureLoader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
//#undef STB_IMAGE_IMPLEMENTATION

#include "render/Texture.h"

#include "utils/asset/AssetFactory.h"
#include "resource/ResourceManager.h"

#if 0
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize.h"
#undef STB_IMAGE_RESIZE_IMPLEMENTATION
#endif

using namespace Grafkit;
using namespace FWdebugExceptions;

DEFINE_EXCEPTION(BitmapLoadException, 1105, "Could not load and create bitmap from file")

// ========================================================================================================================
// Texture buffer generator
// ========================================================================================================================

// Nothing here ATM

// ========================================================================================================================
// Texture buffer generator
// ========================================================================================================================
# if 0
void TextureBufferBuilder::Load(IResourceManager* const & resman, IResource* source)
{
    TextureResRef dstTexture = dynamic_cast<Texture2DRes*>(source);
    if (dstTexture.Invalid())
    {
        THROW_EX(NullPointerException);
    }

    //m_oldResource = static_cast<Ref<Texture2D>>(*dstTexture);

    TextureRef texture = new Texture2D();

    if (dstTexture->Valid()) { dstTexture->Release(); }
    dstTexture->AssingnRef(texture);
}

void TextureBufferBuilder::Initialize(Renderer& render, IResourceManager* const& resman, IResource* source) 
{
    Ref<Texture2D> texture = SafeGetObject(source);

    switch (m_params.type)
    {
    case TextureBufferParams::TB_RGBA:
        texture->Initialize(render, m_params.width, m_params.height);
        break;
    case TextureBufferParams::TB_RGBA32:
        texture->InitializeFloatRGBA(render, m_params.width, m_params.height);
        break;
    case TextureBufferParams::TB_Float:
        texture->InitializeFloat(render, m_params.width, m_params.height);
        break;
    case TextureBufferParams::TB_Depth:
        texture->InitializeDepth(render, m_params.width, m_params.height);
        break;
    }
}

// ========================================================================================================================
// Texture from bitmap loader
// ========================================================================================================================

void TextureFromBitmap::Load(IResourceManager* const & resman, IResource* source)
{
    TextureResRef dstTexture = dynamic_cast<Texture2DRes*>(source);
    if (dstTexture.Invalid())
    {
        THROW_EX(NullPointerException);
    }

    // --- load texture 
    TextureRef texture = new Texture2D();

    LOGGER(Log::Logger().Trace("loading texture from resource"));

    int x = 0, y = 0, ch = 0;
    StreamRef asset = resman->GetAssetFactory()->Get(m_params.filename);

    StreamDataPtr assetData = nullptr;
    size_t assetLength = 0;
    asset->ReadAll(assetLength, assetData);

    // forced RGBA color mode 
    uint8_t* data = stbi_load_from_memory(assetData.get(), assetLength, &x, &y, &ch, 0);

    /// @todo resize to 2^n if needed

    if (!data)
    {
        THROW_EX_DETAILS(BitmapLoadException, stbi_failure_reason());
    }

    texture->Initialize(resman->GetDeviceContext(), new Bitmap(data, x, y, ch));

    // --- xchg texture 
    if (dstTexture->Valid()) { dstTexture->Release(); }

    dstTexture->AssingnRef(texture);
}

void TextureFromBitmap::Initialize(Renderer& render, IResourceManager* const& resman, IResource* source) {
}

// ========================================================================================================================
// 
// ========================================================================================================================


void TextureCubemapFromBitmap::Load(IResourceManager* const & resman, IResource* source)
{
    TextureCubeResRef dstTexture = dynamic_cast<TextureCubeRes*>(source);
    if (dstTexture.Invalid())
    {
        THROW_EX(NullPointerException);
    }

    BitmapRef bitmaps[6];

    for (int i = 0; i < 6; i++)
    {
        int x = 0, y = 0, ch = 0;
        StreamRef asset = resman->GetAssetFactory()->Get(m_params.sourceNames[i]);

        // force rgba mode
        StreamDataPtr assetData = nullptr;
        size_t assetLength = 0;
        asset->ReadAll(assetLength, assetData);
        uint8_t* data = stbi_load_from_memory(assetData.get(), assetLength, &x, &y, &ch, 0);

        /// @todo resize to 2^n if needed

        if (!data)
        {
            THROW_EX_DETAILS(BitmapLoadException, stbi_failure_reason());
        }

        bitmaps[i] = new Bitmap(data, x, y, ch);
    }

    // TODO -> Initialize
    TextureCubeRef texture = new TextureCube();
    texture->Initialize(resman->GetDeviceContext(), new Cubemap(bitmaps));

    // --- xchg texture 
    if (dstTexture->Valid()) { dstTexture->Release(); }

    dstTexture->AssingnRef(texture);
}

void TextureCubemapFromBitmap::Initialize(Renderer& render, IResourceManager* const& resman, IResource* source) {
}

// ========================================================================================================================
// 
// ========================================================================================================================

void TextureNoiseMapBuilder::Load(IResourceManager* const & resman, IResource* source)
{
    TextureResRef dstTexture = dynamic_cast<Texture2DRes*>(source);
    if (dstTexture.Invalid())
    {
        THROW_EX(NullPointerException);
    }

    // --- load texture 
    TextureRef texture = new Texture2D();

    LOGGER(Log::Logger().Trace("loading texture from resource"));

    size_t k = m_params.size * m_params.size * 4;
    uint8_t* data = new uint8_t[k];

    for (int i = 0; i < k; i++) { data[i] = rand() % 255; }

    // TODO: initialize
    texture->Initialize(resman->GetDeviceContext(), new Bitmap(data, m_params.size, m_params.size, 4));

    // --- xchg texture 
    if (dstTexture->Valid()) { dstTexture->Release(); }

    dstTexture->AssingnRef(texture);
}

void TextureNoiseMapBuilder::Initialize(Renderer& render, IResourceManager* const& resman, IResource* source) {
    // Initialize?
}

// ========================================================================================================================
// 
// ========================================================================================================================


void TextureSamplerBuilder::Load(IResourceManager* const & resman, IResource* source)
{
    TextureSamplerResRef sampler = dynamic_cast<Resource<TextureSampler>*>(source);

    if (sampler.Invalid())
        THROW_EX(NullPointerException);

    enum D3D11_TEXTURE_ADDRESS_MODE dxMode;

    switch (m_params.mode)
    {
    case TextureSamplerParams::TGG_Clamping:
        dxMode = D3D11_TEXTURE_ADDRESS_CLAMP;
        break;
    default:
        dxMode = D3D11_TEXTURE_ADDRESS_WRAP;
        break;
    }

    // TODO -> Initialize

    sampler->AssingnRef(new TextureSampler());
    sampler->Get()->Initialize(resman->GetDeviceContext(), dxMode);
}

void TextureSamplerBuilder::Initialize(Renderer& render, IResourceManager* const& resman, IResource* source) {
    // Initialize ?
}

#endif

// ========================================================================================================================
// TextureBufferBuilder
// ========================================================================================================================
void TextureBufferBuilder::Load(const std::shared_ptr<IResource>& resource, ResourceManager& resourceManager, const IAssetFactory & assetFactory) {
}

void TextureBufferBuilder::Initialize(const std::shared_ptr<IResource>& resource, ResourceManager& resourceManager, const IAssetFactory & assetFactory) {
}

// ========================================================================================================================
// TextureFromBitmap
// ========================================================================================================================
void TextureFromBitmap::Load(const std::shared_ptr<IResource>& resource, ResourceManager& resourceManager, const IAssetFactory & assetFactory) {
}

void TextureFromBitmap::Initialize(const std::shared_ptr<IResource>& resource, ResourceManager& resourceManager, const IAssetFactory & assetFactory) {
}

// ========================================================================================================================
// TextureCubemapFromBitmap
// ========================================================================================================================
void TextureCubemapFromBitmap::Load(const std::shared_ptr<IResource>& resource, ResourceManager& resourceManager, const IAssetFactory & assetFactory) {
}

void TextureCubemapFromBitmap::Initialize(const std::shared_ptr<IResource>& resource, ResourceManager& resourceManager, const IAssetFactory & assetFactory) {
}

// ========================================================================================================================
// TextureNoiseMapBuilder
// ========================================================================================================================

void TextureNoiseMapBuilder::Load(const std::shared_ptr<IResource>& resource, ResourceManager& resourceManager, const IAssetFactory & assetFactory) {
}

void TextureNoiseMapBuilder::Initialize(const std::shared_ptr<IResource>& resource, ResourceManager& resourceManager, const IAssetFactory & assetFactory) {
}

// ========================================================================================================================
// TextureSamplerBuilder
// ========================================================================================================================

void TextureSamplerBuilder::Load(const std::shared_ptr<IResource>& resource, ResourceManager& resourceManager, const IAssetFactory & assetFactory) {
}

void TextureSamplerBuilder::Initialize(const std::shared_ptr<IResource>& resource, ResourceManager& resourceManager, const IAssetFactory & assetFactory) {
}

