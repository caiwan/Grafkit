#pragma once


#include "../utils/reference.h"
#include "../utils/resource.h"

#include "renderer.h"
#include "predefs.h"

#define MULTITEXTURE_MAX 16

///@todo a textura generalas, es a textura kezeles legyen kulon fileban valahol, egymastol fuggetlenul, ha lehet.

namespace Grafkit
{

#define TEXTURE_BUCKET "texture"

	// ========================================================================================================================
	
	/**
	A bitmap resource that contains a raw bitmap. 
	*/
	class Bitmap :  public Referencable {
	public:
		Bitmap() : m_bmsize(0), m_ch(0), m_x(0), m_y(0), m_data(nullptr), m_stride(0) {}
		Bitmap(void* data, UINT x, UINT y, UINT ch) : m_bmsize(0), m_ch(ch), m_x(x), m_y(y), m_data(data) { m_bmsize = x*y*ch; m_stride = x*ch; }
		~Bitmap() { free(m_data); }

		virtual void* GetData() const { return m_data; }
		virtual size_t GetSize() const { return m_bmsize; }

		UINT GetX() const { return m_x; }
		UINT GetY() const { return m_y; }
		UINT GetCh() const { return m_ch; }
		UINT GetStride() const { return m_stride; }

		/// @todo resize if needed 

	private:
		void *m_data;
		UINT m_bmsize, m_stride, m_x, m_y, m_ch;
	};

	typedef Ref<Bitmap> BitmapRef;

	// ========================================================================================================================
	
	/** 
	Abstract class for texture generation,
	quick and dirty solution for generate and update texture objects 
	*/

	class ATexture {
	public:
		ATexture();
		virtual  ~ATexture();
	
		//void Initialize();
		void Shutdown();

		ID3D11ShaderResourceView* GetShaderResourceView() const { return this->m_pResourceView; }
		ID3D11RenderTargetView* GetRenderTargetView() const { return this->m_pTargetView; }
		ID3D11Resource* GetTextureResource() const { return this->m_pTexture; }

		void SetRenderTargetView(Renderer & device, size_t id = 0) const;

		void Update(Renderer & device, const void* data);
		void Update(Renderer & device, const BitmapRef bitmap);

	protected:
		void CrateTexture(Renderer & device, DXGI_FORMAT format, int channels = 4, int channelWidth = 1, int w = 0, int h = 0, int d = 0, bool isDynamic = true, bool hasMips = true);
		void UpdateTexture(Renderer & device, const void* data, size_t len);
		
		virtual int GetDimension() = 0;

	protected:
		ID3D11Resource * m_pTexture;
		DXGI_FORMAT m_format;

		size_t m_w, m_h, m_d, m_ch, m_chW;

		ID3D11ShaderResourceView * m_pResourceView;
		ID3D11RenderTargetView * m_pTargetView;

	protected:
		/*int m_mipSlices;
		int m_maxMip;
		int m_mipLevels;*/
		/*D3D11_USAGE m_usage;
		UINT m_CPUWriteFlags;*/
	};
	
	/**
	QnD 1DTextureClass
	with 32 bit float values
	Contains just shortcuts
	*/
	__declspec(align(16)) class Texture1D : public ATexture, virtual public Referencable, public AlignedNew<Texture2D>
	{
	public:
		Texture1D() : ATexture() {}
		~Texture1D() {}
		
		/// inits with 32 bit float 1D texture (array) 
		void Initialize(Renderer & device, size_t w = 0, const float* data = nullptr);

		ID3D11Texture1D* GetTexture1D() { return (ID3D11Texture1D*)this->m_pTexture; }

	protected:
		virtual int GetDimension() { return 1; }
	};

	typedef Ref<Texture1D> Texture1DRef ;

	/**
	QnD 2D Texture class
	Contains just shortcuts
	*/
	__declspec(align(16)) class Texture2D : public ATexture, virtual public Referencable, public AlignedNew<Texture2D>
	{
	public:
		Texture2D() : ATexture() {}
		~Texture2D() {}
		
		/// Inits an RGBA texture with unisned 8 bit per channel each
		void Initialize(Renderer & device, BitmapRef bitmap);
		
		/// Inits an 8bit RGBA texture
		void Initialize(Renderer & device, int w = 0, int h = 0);

		/// Inits an RGBA texture with 32 bit float per channel each
		void InitializeFloatRGBA(Renderer & device, int w = 0, int h = 0);
		
		/// Inits single channel texture with 32 bit float per channel each
		void InitializeFloat(Renderer & device, int w = 0, int h = 0);

		/// Inits depth buffer
		void InitializeDepth(Renderer & device, int w = 0, int h = 0);

		ID3D11Texture2D* GetTexture2D() { return (ID3D11Texture2D*)this->m_pTexture; }

	protected:
		virtual int GetDimension() { return 2; }

	};

	// ========================================================================================================================

	__declspec(align(16)) class TextureSampler : virtual public Referencable, public AlignedNew<Texture2D>
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


}

#define EX_ERROR_TEXTURE 1200

DEFINE_EXCEPTION(TextureCreateException, EX_ERROR_TEXTURE + 1, "Could not create texture")
DEFINE_EXCEPTION(ShaderResourceViewException, EX_ERROR_TEXTURE + 2, "Could not create shader resource view")
DEFINE_EXCEPTION(RenderTargetViewException, EX_ERROR_TEXTURE + 3, "Could not create render target view")
DEFINE_EXCEPTION(NoRenderTargetViewException, EX_ERROR_TEXTURE + 4, "Texture was not created as bare, and has no render target view")
DEFINE_EXCEPTION(SamplerStateCreateException, EX_ERROR_TEXTURE + 5, "Could not create sampler state")

