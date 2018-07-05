#pragma once

// #include "../utils/asset.h"
#include "../utils/resource.h"
#include "../utils/ResourceBuilder.h"
#include "../utils/ResourceManager.h"

#include "../utils/exceptions.h"

namespace Grafkit{

	/**
	Texture generator interface
	*/
	class ITexture1DBuilder : public Grafkit::IResourceBuilder
	{
	public:
		ITexture1DBuilder(std::string name, std::string source_name) : IResourceBuilder(name, source_name) {}
		ITexture1DBuilder(std::string name) : IResourceBuilder(name) {}
		virtual ~ITexture1DBuilder() {}

	    void Load(Grafkit::IResourceManager * const & resman, Grafkit::IResource * source) override = 0;

	    IResource* NewResource() override;
	};

	class ITexture2DBuilder : public Grafkit::IResourceBuilder
	{
	public:
		ITexture2DBuilder(std::string name, std::string source_name) : IResourceBuilder(name, source_name) {}
		ITexture2DBuilder(std::string name) : IResourceBuilder(name) {}
		virtual ~ITexture2DBuilder() {}

	    void Load(Grafkit::IResourceManager * const & resman, Grafkit::IResource * source) override = 0;

	    IResource* NewResource() override;
	};

	/*
	*/
	class TextureBufferBuilder : public ITexture2DBuilder{
	public:
		enum Type_E {
			TB_RGBA,
			TB_RGBA32,
			TB_Float,
			TB_Depth
		};

	public:
		TextureBufferBuilder(std::string name, TextureBufferBuilder::Type_E type);
		TextureBufferBuilder(std::string name, TextureBufferBuilder::Type_E type, uint16_t w, uint16_t h);

	    void Load(Grafkit::IResourceManager * const & resman, Grafkit::IResource * source) override;
	
	private:
		uint16_t m_w, m_h;
		Type_E m_type;
	};

	/**
	*/
	class TextureFromBitmap : public ITexture2DBuilder
	{
	public:
		//TextureFromBitmap(std::string source_name);
		TextureFromBitmap(std::string name, std::string source_name);
		TextureFromBitmap(std::string name);
		~TextureFromBitmap();

		///@todo implement resize
		void Resize(int x, int y) { m_w = x, m_h = y; }

	    void Load(Grafkit::IResourceManager * const & resman, Grafkit::IResource * source) override;

	protected:
		int m_w, m_h;
	};

	class TextureCubemapFromBitmap : public ITexture2DBuilder {
	public:
		TextureCubemapFromBitmap(std::string name, std::string source_posx, std::string source_negx, std::string source_posy, std::string source_negy, std::string source_posz, std::string source_negz);
		~TextureCubemapFromBitmap();

	    void Load(Grafkit::IResourceManager * const & resman, Grafkit::IResource * source) override;

	    IResource* NewResource() override;
	private:
		std::string m_sourceNames[6];
	};

	class TextureNoiseMap : public TextureFromBitmap {
	public:
		TextureNoiseMap(size_t size);
		TextureNoiseMap(std::string name, size_t size);
	    void Load(Grafkit::IResourceManager * const & resman, Grafkit::IResource * source) override;
	private:
		size_t m_size;
	};

#define TS_NAME_CLAMP "TextureSamplerClamp"
#define TS_NAME_WRAP "TextureSamplerWrap"

	class TextureSamplerBuilder : public Grafkit::IResourceBuilder {
	public:
		enum Type_E {
			TGG_Clamping,
			TGG_Wrapping
		};

		TextureSamplerBuilder(TextureSamplerBuilder::Type_E type);

	    void Load(Grafkit::IResourceManager * const & resman, Grafkit::IResource * source) override;

	    IResource* NewResource() override; 

	private:
		D3D11_TEXTURE_ADDRESS_MODE m_mode;

	};

}

