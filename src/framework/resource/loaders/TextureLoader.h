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

    class TextureBufferBuilder : public ResourceBuilder<Texture2D, TextureBufferParams>
    {
    public:
        TextureBufferBuilder() {
        }

        explicit TextureBufferBuilder(const TextureBufferParams& params)
            : ResourceBuilder<Texture2D, TextureBufferParams>(params) {
        }

        TextureBufferBuilder(const std::string& name, const std::string& uuid, const TextureBufferParams& params)
            : ResourceBuilder<Texture2D, TextureBufferParams>(name, uuid, params) {
        }

        void Load(IResourceManager* const& resman, IResource* source) override;
        void Initialize(Renderer& render, IResourceManager* const& resman, IResource* source) override;

        SERIALIZE(TextureBufferBuilder, 1, ar)
        {
            assert(0);
        }
        //protected:
        //    void Serialize(Archive& ar) override;
    };

    /**
    */

    struct TextureBitmapParams
    {
        std::string filename;
        // resize?
    };

    class TextureFromBitmap : public ResourceBuilder<Texture2D, TextureBitmapParams>
    {
    public:

        TextureFromBitmap() {
        }

        explicit TextureFromBitmap(const TextureBitmapParams& params)
            : ResourceBuilder<Texture2D, TextureBitmapParams>(params) {
        }

        TextureFromBitmap(const std::string& name, const std::string& uuid, const TextureBitmapParams& params)
            : ResourceBuilder<Texture2D, TextureBitmapParams>(name, uuid, params) {
        }

        void Load(IResourceManager* const& resman, IResource* source) override;
        void Initialize(Renderer& render, IResourceManager* const& resman, IResource* source) override;

        SERIALIZE(TextureFromBitmap, 1, ar)
        {
            assert(0);
        }

        //protected:
        //    void Serialize(Archive& ar) override;
    };


    /**
     *
     */

    struct TextureCubemapParams
    {
        std::array<std::string, 6> sourceNames;
    };

    class TextureCubemapFromBitmap : public ResourceBuilder<TextureCube, TextureCubemapParams>
    {
    public:
        TextureCubemapFromBitmap() {
        }

        explicit TextureCubemapFromBitmap(const TextureCubemapParams& params)
            : ResourceBuilder<TextureCube, TextureCubemapParams>(params) {
        }

        TextureCubemapFromBitmap(const std::string& name, const std::string& uuid, const TextureCubemapParams& params)
            : ResourceBuilder<TextureCube, TextureCubemapParams>(name, uuid, params) {
        }

        void Load(IResourceManager* const& resman, IResource* source) override;
        void Initialize(Renderer& render, IResourceManager* const& resman, IResource* source) override;

        SERIALIZE(TextureCubemapFromBitmap, 1, ar)
        {
            assert(0);
        }

        //protected:
        //    void Serialize(Archive& ar) override;
    };

    /*
     *
     */

    struct TextureNoiseParams
    {
        uint16_t size;
    };

    class TextureNoiseMapBuilder : public ResourceBuilder<Texture2D, TextureNoiseParams>
    {
    public:

        TextureNoiseMapBuilder() {
        }

        explicit TextureNoiseMapBuilder(const TextureNoiseParams& params)
            : ResourceBuilder<Texture2D, TextureNoiseParams>(params) {
        }

        TextureNoiseMapBuilder(const std::string& name, const std::string& uuid, const TextureNoiseParams& params)
            : ResourceBuilder<Texture2D, TextureNoiseParams>(name, uuid, params) {
        }

        void Load(IResourceManager* const& resman, IResource* source) override;
        void Initialize(Renderer& render, IResourceManager* const& resman, IResource* source) override;

        SERIALIZE(TextureNoiseMapBuilder, 1, ar)
        {
            assert(0);
        }
        //protected:
        //    void Serialize(Archive& ar) override;
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

    class TextureSamplerBuilder : public ResourceBuilder<TextureSampler, TextureSamplerParams>
    {
    public:

        TextureSamplerBuilder() {
        }

        explicit TextureSamplerBuilder(const TextureSamplerParams& params)
            : ResourceBuilder<TextureSampler, TextureSamplerParams>(params) {
        }

        TextureSamplerBuilder(const std::string& name, const std::string& uuid, const TextureSamplerParams& params)
            : ResourceBuilder<TextureSampler, TextureSamplerParams>(name, uuid, params) {
        }


        void Load(IResourceManager* const& resman, IResource* source) override;
        void Initialize(Renderer& render, IResourceManager* const& resman, IResource* source) override;

        SERIALIZE(TextureSamplerBuilder, 1, ar)
        {
            assert(0);
        }

        //protected:
        //    void Serialize(Archive& ar) override;
    };
}
