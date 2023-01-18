#pragma once

#include <string>
#include "renderer.h"

#include "renderassets.h"

#define MULTITEXTURE_MAX 16

namespace FWrender
{
	/**
	Texture generator interface
	*/
	class ITextureGenerator : virtual public FWrender::IRenderAssetGenerator
	{
	public: 
		ITextureGenerator() {}
		virtual ~ITextureGenerator() {} 
		
		virtual TextureRef operator() (ID3D11Device* device, TextureRef in) = 0;
	};

	/**
	Texture class
	*/
	class Texture : virtual public Referencable, public IRenderAsset
	{
	public:
		Texture();
		Texture(const Texture&);
		~Texture();

		// bool Initialize(ID3D11Device* device, WCHAR* filename);
		void Initialize(ID3D11Device* device, void* data, int x, int y, int ch = 4);
		void Shutdown();

		ID3D11Texture2D* GetTexture2D() { return this->m_pTex; }
		ID3D11ShaderResourceView* GetTextureResource() { return this->m_pResourceView; }
		ID3D11SamplerState*	GetSamplerState() { return this->m_pSamplerState; }

		operator ID3D11Texture2D* () { return this->m_pTex; }
		operator ID3D11ShaderResourceView* () { return this->m_pResourceView; }

	private:
		ID3D11Texture2D * m_pTex;
		ID3D11ShaderResourceView* m_pResourceView;
		ID3D11SamplerState *m_pSamplerState;
	};

	class TextureGenFromBitmap : public ITextureGenerator
	{
		public:
			TextureGenFromBitmap(const WCHAR * const filename);
			TextureGenFromBitmap(const char * const filename);
			~TextureGenFromBitmap();

			void Resize(int x, int y) { m_w = x, m_h = y; }
			virtual TextureRef operator() (ID3D11Device* device, TextureRef in = nullptr);

		protected:
			std::string m_filename;
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
	TextureGenFromBitmap txgen(filename);\
	txgen(render, input);\
}

DEFINE_EXCEPTION(BitmapLoadException, 1105, "Could not load and create bitmap from file")
DEFINE_EXCEPTION(TextureCreateException, 1105, "Could not create texture")
DEFINE_EXCEPTION(ShaderResourceViewException, 1105, "Could not create shader resource view")
DEFINE_EXCEPTION(SamplerStateCreateException, 1105, "Could not create sampler state")
