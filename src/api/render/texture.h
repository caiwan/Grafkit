#pragma once

#include <string>
#include "renderer.h"

#include "../core/renderassets.h"
#include "../core/assets.h"

#define MULTITEXTURE_MAX 16

///@todo a textura generalas, es a textura kezeles legyen kulon fileban valahol, egymastol fuggetlenul, ha lehet.

namespace FWrender
{
	/**
	A bitmap resource that contains a raw bitmap. This ig enerated by the generated, and loaded into the texture object.
	*/
	class BitmapResource : public FWassets::IResource {
	public:
		BitmapResource() : m_bmsize(0), m_ch(0), m_x(0), m_y(0), m_data(nullptr) {}
		BitmapResource(void* data, size_t x, size_t y, size_t ch) : m_bmsize(0), m_ch(ch), m_x(x), m_y(y), m_data(data) { m_bmsize = x*y*ch; m_stride = x*ch; }
		~BitmapResource() { free(m_data); }

		virtual void* GetData() { return m_data; }
		virtual size_t GetSize() { return m_bmsize; }

		size_t GetX() { return m_x; }
		size_t GetY() { return m_y; }
		size_t GetCh() { return m_ch; }
		size_t GetStride() { return m_stride; }

		/// @todo resize if needed 

	private:
		void *m_data;
		size_t m_bmsize, m_stride, m_x, m_y, m_ch;
	};

	typedef Ref<BitmapResource> BitmapResourceRef;

	/**
	Texture generator interface
	*/
	class ITextureBuilder : public FWassets::IRenderAssetBuilder
	{
	public: 
		ITextureBuilder(TextureRef * const in) : IRenderAssetBuilder(), m_in(in) {}
		virtual ~ITextureBuilder() {} 
		
		virtual void operator() (FWassets::IRenderAssetManager * const & assman) = 0;

	protected:
		TextureRef * const m_in;
	};

	/**
	Texture class
	*/
	class Texture : virtual public Referencable, public FWassets::IRenderAsset
	{
	public:
		Texture();
		Texture(const Texture&);
		~Texture();

		void Initialize(ID3D11Device * const & device, BitmapResourceRef bitmap);
		void Shutdown();

		ID3D11Texture2D* GetTexture2D() { return this->m_pTex; }
		ID3D11ShaderResourceView* GetTextureResource() { return this->m_pResourceView; }
		ID3D11SamplerState*	GetSamplerState() { return this->m_pSamplerState; }

		operator ID3D11Texture2D* () { return this->m_pTex; }
		operator ID3D11ShaderResourceView* () { return this->m_pResourceView; }

		virtual enum RA_type_e GetBucketID() { return FWassets::IRenderAsset::RA_TYPE_Texture; }

	private:
		ID3D11Texture2D * m_pTex;
		ID3D11ShaderResourceView* m_pResourceView;
		ID3D11SamplerState *m_pSamplerState;
	};

	class TextureFromBitmap : public ITextureBuilder
	{
		public:
			TextureFromBitmap(FWassets::IResourceRef resource, TextureRef * const in);
			~TextureFromBitmap();

			void Resize(int x, int y) { m_w = x, m_h = y; }
			virtual void operator() (FWassets::IRenderAssetManager * const & assman);

		protected:
			FWassets::IResourceRef m_resource;
			int m_w, m_h;
	};

	/** Render target texture 
	*/
	class RenderTarget : public Referencable
	{
	};
}

// load texture from the texture repository 
#define LOAD_TEXTURE(render, filename, input)\
{\
	TextureFromBitmap txgen(filename);\
	txgen(render, input);\
}

DEFINE_EXCEPTION(BitmapLoadException, 1105, "Could not load and create bitmap from file")
DEFINE_EXCEPTION(TextureCreateException, 1105, "Could not create texture")
DEFINE_EXCEPTION(ShaderResourceViewException, 1105, "Could not create shader resource view")
DEFINE_EXCEPTION(SamplerStateCreateException, 1105, "Could not create sampler state")
