#pragma once


#include "../utils/reference.h"
#include "../utils/resource.h"

#include "renderer.h"

#define MULTITEXTURE_MAX 16

///@todo a textura generalas, es a textura kezeles legyen kulon fileban valahol, egymastol fuggetlenul, ha lehet.

namespace Grafkit
{

#define TEXTURE_BUCKET "texture"

	// ========================================================================================================================

	/**
	A bitmap resource that contains a raw bitmap. This ig enerated by the generated, and loaded into the texture object.
	*/
	class Bitmap : public Referencable {
	public:
		Bitmap() : m_bmsize(0), m_ch(0), m_x(0), m_y(0), m_data(nullptr), m_stride(0) {}
		Bitmap(void* data, UINT x, UINT y, UINT ch) : m_bmsize(0), m_ch(ch), m_x(x), m_y(y), m_data(data) { m_bmsize = x*y*ch; m_stride = x*ch; }
		~Bitmap() { free(m_data); }

		virtual void* GetData() { return m_data; }
		virtual size_t GetSize() { return m_bmsize; }

		UINT GetX() { return m_x; }
		UINT GetY() { return m_y; }
		UINT GetCh() { return m_ch; }
		UINT GetStride() { return m_stride; }

		/// @todo resize if needed 

	private:
		void *m_data;
		UINT m_bmsize, m_stride, m_x, m_y, m_ch;
	};

	typedef Ref<Bitmap> BitmapResourceRef;

	// ========================================================================================================================

	/**
	Texture class
	*/
	__declspec(align(16)) class Texture : virtual public Referencable, public AlignedNew<Texture>
	{
	public:
		Texture();
		~Texture();

		void Initialize(Renderer & device, BitmapResourceRef bitmap);
		void Initialize(Renderer & device, size_t w = 0, size_t h = 0);
		void Shutdown();

		ID3D11Texture2D* GetTexture2D() { return this->m_pTex; }
		ID3D11ShaderResourceView* GetTextureResource() { return this->m_pResourceView; }
		ID3D11RenderTargetView* GetRenderTargetView() { return this->m_pRenderTargetView; }

		void SetRenderTargetView(Renderer & device, size_t id =0);

	private:
		ID3D11Texture2D * m_pTex;
		ID3D11ShaderResourceView* m_pResourceView;
		ID3D11RenderTargetView* m_pRenderTargetView;
	};

	// ========================================================================================================================

	typedef Ref<Texture> TextureRef;
	typedef Resource<Texture> TextureRes;
	typedef Ref<TextureRes> TextureResRef;

	// ========================================================================================================================

	__declspec(align(16)) class TextureSampler : virtual public Referencable, public AlignedNew<Texture>
	{
	public:
		TextureSampler();
		~TextureSampler();

		void Initialize(Renderer & device);
		void Shutdown();

		ID3D11SamplerState*	GetSamplerState() { return this->m_pSamplerState; }

	private:
		ID3D11SamplerState *m_pSamplerState;
	};

	// ========================================================================================================================

	typedef Ref<TextureSampler> TextureSamplerRef;
	typedef Grafkit::Resource<TextureSampler> TextureSamplerResource;
	typedef Ref<TextureSamplerResource> TextureSamplerResourceRef;

	// ========================================================================================================================
}

#define EX_ERROR_TEXTURE 1200

DEFINE_EXCEPTION(TextureCreateException, EX_ERROR_TEXTURE + 1, "Could not create texture")
DEFINE_EXCEPTION(ShaderResourceViewException, EX_ERROR_TEXTURE + 2, "Could not create shader resource view")
DEFINE_EXCEPTION(RenderTargetViewException, EX_ERROR_TEXTURE + 3, "Could not create render target view")
DEFINE_EXCEPTION(NoRenderTargetViewException, EX_ERROR_TEXTURE + 4, "Texture was not created as bare, and has no render target view")
DEFINE_EXCEPTION(SamplerStateCreateException, EX_ERROR_TEXTURE + 5, "Could not create sampler state")

