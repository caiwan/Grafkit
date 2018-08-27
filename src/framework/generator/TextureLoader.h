#pragma once

// #include "../utils/asset.h"
#include "../utils/resource.h"
#include "../utils/ResourceBuilder.h"
#include "../utils/ResourceManager.h"

#include "../utils/exceptions.h"

namespace Grafkit {

    /**
    Texture generator interface
    */
    class ITexture1DBuilder : public IResourceBuilder
    {
    public:
        explicit ITexture1DBuilder(std::string name, std::string sourceName = "", std::string uuid = "") : IResourceBuilder(name, sourceName, uuid) {}
        virtual ~ITexture1DBuilder() {}

        void Load(IResourceManager * const & resman, IResource * source) override = 0;

        IResource* NewResource() override;
    };

    class ITexture2DBuilder : public IResourceBuilder
    {
    public:
        explicit ITexture2DBuilder(std::string name, std::string sourceName = "", std::string uuid = "") : IResourceBuilder(name, sourceName, uuid) {}
        virtual ~ITexture2DBuilder() {}

        void Load(IResourceManager * const & resman, IResource * source) override = 0;

        IResource* NewResource() override;
    };

    /*
    */
    class TextureBufferBuilder : public ITexture2DBuilder {
    public:
        enum TextureTypeE {
            TB_RGBA,
            TB_RGBA32,
            TB_Float,
            TB_Depth
        };

        TextureBufferBuilder(std::string name, TextureTypeE type);
        TextureBufferBuilder(std::string name, TextureTypeE type, uint16_t w, uint16_t h);

        void Load(IResourceManager * const & resman, IResource * source) override;

    private:
        uint16_t m_w, m_h;
        TextureTypeE m_type;
    };

    /**
    */
    class TextureFromBitmap : public ITexture2DBuilder
    {
    public:
        explicit TextureFromBitmap(std::string name, std::string sourceName = "", std::string uuid = "");
        ~TextureFromBitmap();

        ///@todo implement resize
        void Resize(int x, int y) { m_w = x, m_h = y; }

        void Load(IResourceManager * const & resman, IResource * source) override;

    protected:
        int m_w, m_h;
    };

    class TextureCubemapFromBitmap : public ITexture2DBuilder {
    public:
        explicit TextureCubemapFromBitmap(std::string name, std::string source_posx, std::string source_negx, std::string source_posy, std::string source_negy, std::string source_posz, std::string source_negz, std::string uuid ="");
        explicit TextureCubemapFromBitmap(std::string name, std::vector<std::string> sourceNames, std::string uuid = "");
        ~TextureCubemapFromBitmap();

        void Load(IResourceManager * const & resman, IResource * source) override;

        IResource* NewResource() override;
    private:
        std::string m_sourceNames[6];
    };

    class TextureNoiseMap : public TextureFromBitmap {
    public:
        explicit TextureNoiseMap(size_t size, std::string uuid = "");
        explicit TextureNoiseMap(std::string name, size_t size, std::string uuid = "");
        void Load(IResourceManager * const & resman, IResource * source) override;
    private:
        size_t m_size;
    };

#define TS_NAME_CLAMP "TextureSamplerClamp"
#define TS_NAME_WRAP "TextureSamplerWrap"

    class TextureSamplerBuilder : public IResourceBuilder {
    public:
        enum Type_E {
            TGG_Clamping,
            TGG_Wrapping
        };

        TextureSamplerBuilder(Type_E type);

        void Load(IResourceManager * const & resman, IResource * source) override;

        IResource* NewResource() override;

    private:
        D3D11_TEXTURE_ADDRESS_MODE m_mode;

    };

}

