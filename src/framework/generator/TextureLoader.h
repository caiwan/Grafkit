#pragma once

#include "../render/texture.h"

// #include "../utils/asset.h"
#include "../utils/resource.h"
#include "../utils/ResourceBuilder.h"
#include "../utils/ResourceManager.h"

#include "../utils/exceptions.h"

namespace Grafkit{

	/**
	Texture generator interface
	*/
	class ITextureBuilder : public Grafkit::IResourceBuilder
	{
	public:
		ITextureBuilder(std::string name, std::string source_name) : IResourceBuilder(name, source_name) {}
		virtual ~ITextureBuilder() {}

		virtual void Load(Grafkit::IResourceManager * const & assman, Grafkit::IResource * source) = 0;

		virtual IResource* NewResource();
	};

	/**
	*/
	class TextureFromBitmap : public ITextureBuilder
	{
	public:
		//TextureFromBitmap(std::string source_name);
		TextureFromBitmap(std::string name, std::string source_name);
		~TextureFromBitmap();

		///@todo implement resize
		void Resize(int x, int y) { m_w = x, m_h = y; }

		virtual void Load(Grafkit::IResourceManager * const & resman, Grafkit::IResource * source);

	protected:
		int m_w, m_h;
	};

	class TextureCubemapFromBitmap : public ITextureBuilder {
	public:
		TextureCubemapFromBitmap(std::string name, std::string source_posx, std::string source_negx, std::string source_posy, std::string source_negy, std::string source_posz, std::string source_negz);
		~TextureCubemapFromBitmap();

		virtual void Load(Grafkit::IResourceManager * const & resman, Grafkit::IResource * source);

		virtual IResource* NewResource();
	private:
		std::string m_sourceNames[6];
	};

	class TextureNoiseMap : public TextureFromBitmap {
	public:
		TextureNoiseMap(size_t size);
		virtual void Load(Grafkit::IResourceManager * const & resman, Grafkit::IResource * source);
	private:
		size_t m_size;
	};

#define TS_NAME_CLAMP "TextureSamplerClamp"
#define TS_NAME_WRAP "TextureSamplerWrap"

	class TextureSamplerGenerator : public Grafkit::IResourceBuilder {
	public:
		enum Type_e {
			TGG_Clamping,
			TGG_Wrapping
		};

		TextureSamplerGenerator(TextureSamplerGenerator::Type_e type);

		virtual void Load(Grafkit::IResourceManager * const & assman, Grafkit::IResource * source);

		virtual IResource* NewResource(); 

	private:
		D3D11_TEXTURE_ADDRESS_MODE m_mode;

	};

}

