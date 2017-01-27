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
	Texture loader from bitmap
	Ide kijegyzetelem, hogy mije van ennek, es hogyan kell vele banni:
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
}

