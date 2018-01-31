#pragma once


#include "../utils/reference.h"
#include "../utils/resource.h"

#include "renderer.h"
#include "predefs.h"

#define MULTITEXTURE_MAX 16

///@todo a textura generalas, es a textura kezeles legyen kulon fileban valahol, egymastol fuggetlenul, ha lehet.

namespace Grafkit
{
	// ========================================================================================================================

	/**
	A bitmap resource that contains a raw bitmap.
	*/
	class Bitmap : public Referencable {
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

	class Cubemap : public Referencable {
	public:
		Cubemap() {
		}

		Cubemap(BitmapRef bitmaps[6]) {
			for (int i = 0; i < 6; i++)
				m_cube[i] = bitmaps[i];
		}

		BitmapRef GetBitmap(int i) { return m_cube[i % 6]; }

		BitmapRef GetPosX() const { return m_cube[0]; }
		BitmapRef GetNegX() const { return m_cube[1]; }
		BitmapRef GetPosY() const { return m_cube[2]; }
		BitmapRef GetNegY() const { return m_cube[3]; }
		BitmapRef GetPosZ() const { return m_cube[4]; }
		BitmapRef GetNegZ() const { return m_cube[5]; }

	private:
		//BitmapRef m_posx, m_negx, m_posy, m_negy, m_posz, m_negz;
		BitmapRef m_cube[6];
	};


	typedef Ref<Cubemap> CubemapRef;

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

		operator ID3D11ShaderResourceView*() const { return this->m_pResourceView; }

		operator ID3D11RenderTargetView*() const { return this->m_pTargetView; }
		operator ID3D11Resource*() const { return this->m_pTexture; }

		// Ezt kurvara ki kell innen baszni
		//void SetRenderTargetView(Renderer & device, size_t id = 0) const;

		void Update(Renderer & device, const void* data, size_t index = 0);
		void Update(Renderer & device, const BitmapRef bitmap, size_t index = 0);
		//void Update(Renderer & device, const CubemapRef cubemap);

	protected:
		//virtual void CrateTexture(Renderer & device, DXGI_FORMAT format, int channels = 4, int channelWidth = 1, int w = 0, int h = 0, int d = 0, bool isDynamic = true, bool hasMips = true, bool cubemap = false);
		virtual void UpdateTexture(Renderer & device, const void* data, size_t len, size_t subresource = 0, size_t offset = 0);

		// convert 3 component bitmap to 4
		void CopyChannel3To4(const void *in, void *out, size_t w, size_t h);

		virtual int GetDimension() = 0;

	protected:
		ID3D11Resource * m_pTexture;

		void SetDimm(size_t w = 0, size_t h = 0, size_t d = 0, size_t ch = 0, size_t chw = 0) {
			width = w, height = h, depth = d, channels = ch, channelWidth = chw;
		}

		// width, height, depth
		size_t width, height, depth;
		// number of channels, one channel size in byte
		size_t channels, channelWidth;

		ID3D11ShaderResourceView * m_pResourceView;
		ID3D11RenderTargetView * m_pTargetView;

	protected:
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
		void Initialize(Renderer & device, size_t w, size_t components = 1);
		void Initialize(Renderer & device, size_t w, const float* data);
		void Initialize(Renderer & device, size_t w, const float4* data);

		ID3D11Texture1D* GetTexture1D() { return (ID3D11Texture1D*)this->m_pTexture; }

	protected:
		void CreateTexture(Renderer & device, DXGI_FORMAT format, size_t w, const void * initaldata = nullptr);

		virtual int GetDimension() { return 1; }
	};

	typedef Ref<Texture1D> Texture1DRef;

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
		void CreateTextureBitmap(Renderer & device, DXGI_FORMAT format, int channels = 4, int w = 0, int h = 0, const void * initialData = nullptr);
		void CreateTextureTarget(Renderer & device, DXGI_FORMAT format, int channels = 4, int channelWidth = 1, int w = 0, int h = 0);

		virtual int GetDimension() { return 2; }

	};

	typedef Ref<Texture2D> Texture2DRef;

	__declspec(align(16)) class TextureCube : public ATexture, virtual public Referencable, public AlignedNew<Texture2D>
	{
	public:
		TextureCube() : ATexture() {}
		~TextureCube() {}

		void Initialize(Renderer &device, CubemapRef cubemap);

		void* GetTexture2D() { return (ID3D11Texture2D*)this->m_pTexture; }

	protected:
		virtual int GetDimension() { return 2; }
		//virtual void CrateTexture(Renderer & device, DXGI_FORMAT format, int channels = 4, int channelWidth = 1, int w = 0, int h = 0, int d = 0, bool isDynamic = true, bool hasMips = true, bool cubemap = false);

		void **m_initial_data;
	};

	// ========================================================================================================================

	__declspec(align(16)) class TextureSampler : virtual public Referencable, public AlignedNew<Texture2D>
	{
	public:
		TextureSampler();
		~TextureSampler();

		void Initialize(Renderer & device, D3D11_TEXTURE_ADDRESS_MODE mode = D3D11_TEXTURE_ADDRESS_WRAP);
		void Shutdown();

		ID3D11SamplerState*	GetSamplerState() { return this->m_pSamplerState; }
		operator ID3D11SamplerState*() { return this->m_pSamplerState; }

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

