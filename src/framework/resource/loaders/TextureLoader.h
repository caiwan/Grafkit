#pragma once

#include "common.h"

// #include "core/asset.h"
#include "core/resource.h"
#include "resource/ResourceBuilder.h"
#include "resource/ResourceManager.h"

#include "core/exceptions.h"

namespace Grafkit
{
    // ==============================================================================================================================
    /**
    1D textures
    */

    // ==============================================================================================================================
    /**
    2D textures
    */

    struct TextureBufferParams
    {
        enum TextureTypeE
        {
            TB_RGBA,
            TB_RGBA32,
            TB_Float,
            TB_Depth
        };

        std::string name;
        TextureTypeE type;
        uint16_t width;
        uint16_t height;
    };

    class TextureBufferBuilder : public ResourceLoader<Texture2D, TextureBufferParams>
    {
    public:

        TextureBufferBuilder(const Uuid& id, const TextureBufferParams& params)
            : ResourceLoader<Texture2D, TextureBufferParams>(id, params) {
        }

    protected:
        void Load(const std::shared_ptr<IResource>& resource, ResourceManager& resourceManager, const IAssetFactory & assetFactory) override;
        void Initialize(const std::shared_ptr<IResource>& resource, ResourceManager& resourceManager, const IAssetFactory & assetFactory) override;
    };

    /**
    */

    struct TextureBitmapParams
    {
        std::string name;
        std::string filename;
        // resize?
    };

    class TextureFromBitmap : public ResourceLoader<Texture2D, TextureBitmapParams>
    {
    public:
        TextureFromBitmap(const Uuid &id, const TextureBitmapParams& params)
            : ResourceLoader<Texture2D, TextureBitmapParams>(id, params) {
        }

    protected:
        void Load(const std::shared_ptr<IResource>& resource, ResourceManager& resourceManager, const IAssetFactory & assetFactory) override;
        void Initialize(const std::shared_ptr<IResource>& resource, ResourceManager& resourceManager, const IAssetFactory & assetFactory) override;
    };


    /**
     *
     */

    struct TextureCubemapParams
    {
        std::string name;
        std::array<std::string, 6> sourceNames;
    };

    class TextureCubemapFromBitmap : public ResourceLoader<TextureCube, TextureCubemapParams>
    {
    public:

        TextureCubemapFromBitmap(const Uuid &id, const TextureCubemapParams& params)
            : ResourceLoader<TextureCube, TextureCubemapParams>(id, params) {
        }

    protected:
        void Load(const std::shared_ptr<IResource>& resource, ResourceManager& resourceManager, const IAssetFactory & assetFactory) override;
        void Initialize(const std::shared_ptr<IResource>& resource, ResourceManager& resourceManager, const IAssetFactory & assetFactory) override;
    };

    /*
     *
     */

    struct TextureNoiseParams
    {
        std::string name;
        uint16_t size;
    };

    class TextureNoiseMapBuilder : public ResourceLoader<Texture2D, TextureNoiseParams>
    {
    public:


        TextureNoiseMapBuilder(const Uuid &id, const TextureNoiseParams& params)
            : ResourceLoader<Texture2D, TextureNoiseParams>(id, params) {
        }

    protected:
        void Load(const std::shared_ptr<IResource>& resource, ResourceManager& resourceManager, const IAssetFactory & assetFactory) override;
        void Initialize(const std::shared_ptr<IResource>& resource, ResourceManager& resourceManager, const IAssetFactory & assetFactory) override;
    };

    /**
     *
     */

     //#define TS_NAME_CLAMP "TextureSamplerClamp"
     //#define TS_NAME_WRAP "TextureSamplerWrap"

    struct TextureSamplerParams
    {
        enum AddressMode
        {
            TGG_Clamping = 0,
            TGG_Wrapping = 1
        };

        AddressMode mode;
    };

    class TextureSamplerBuilder : public ResourceLoader<TextureSampler, TextureSamplerParams>
    {
    public:

        TextureSamplerBuilder(const Uuid& id, const TextureSamplerParams& params)
            : ResourceLoader<TextureSampler, TextureSamplerParams>(id, params) {
        }

    protected:
        void Load(const std::shared_ptr<IResource>& resource, ResourceManager& resourceManager, const IAssetFactory & assetFactory) override;
        void Initialize(const std::shared_ptr<IResource>& resource, ResourceManager& resourceManager, const IAssetFactory & assetFactory) override;
    };
}
