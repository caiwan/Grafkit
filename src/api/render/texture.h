#pragma once

#include "reference.h"

#include "renderer.h"
#include "../core/resource.h"

#define MULTITEXTURE_MAX 16

///@todo a textura generalas, es a textura kezeles legyen kulon fileban valahol, egymastol fuggetlenul, ha lehet.

namespace Grafkit
{
	class Texture;

	typedef Ref<Texture> TextureRef_t;
	typedef TextureRef_t TextureRef;

	class TextureRes;
	typedef Ref<TextureRes> TextureResRef_t;

	class TextureResRef;

#define TEXTURE_BUCKET "texture"

	// ========================================================================================================================
	///@todo ezt valahogy ujra kell majd gondolni; nyilvan nem szep, es elegans megoldas ez, sajnos
	///@todo erre a mokara fel lehetne talalni egy makrot, ami megcsinalja mindezt
	/**
	`DHOM` - A wrapper class to embed volatile textures
	- A Render asset interacet, es a textura objektumot egyesiti
	- A dolog ugy nez ki, hogy ennek van egy `Texture* ptr` adattagja.
	- Megvalositja a render assetet, es emellett referencia szamlaloja a texturanak
	- Rendelkezik a referenciatipus tagfuggvenyeivel
	*/

	class TextureRes : public Grafkit::IResource, public TextureRef_t
	{
		friend class TextureResRef;
	public:
		TextureRes() : IResource() {}
		~TextureRes() {} ///@todo nem artana tudni, hogyan mukodnek a destruktorok ilyen extem korulmenyek kozott

		virtual const char* GetBucketID() { return TEXTURE_BUCKET; }
	};

	/**
	Egy wrapper, *ami elviekben segit elerni* a texurat a texture asset objektum belsejebol. A `TextureRes` referenciatipusat orokiti tovabb.
	- van egy ptr adattagja, ami a `TextureRes`-re mutat, illetve elvegzi annak referenciaszamlalasat
	- a `this->ptr` ide mutat, es a this->ptr->ptr mutat a Texturarar kozvetlen
	- ezt delegacio segiti

	**Remeljuk osszeszoritott farpofakkal, hogy mindez mukodik.**
	- Azt kell eszben tartani, hogy a `.` operator a refet eri el, ami az assetet fedezi
	- a `->` operator a texture refet eri el, ami a texturat fedezi.

	*/
	class TextureResRef : public TextureResRef_t
	{
		// add conversion to Texture*
	public:
		TextureResRef() {}
		TextureResRef(TextureResRef& other) { this->AssingnRef(other); }
		TextureResRef(TextureRes* other) { this->AssingnRef(other); }

		operator Texture* () { return this->ptr->ptr; }
		operator TextureRef () { return this->ptr->ptr; }
		TextureResRef& operator = (Texture* in_ptr) { this->ptr->AssingnRef(in_ptr); return *this; }
		TextureResRef& operator = (TextureRes* in_ptr) { this->AssingnRef(in_ptr); return *this; }
		TextureResRef& operator = (TextureResRef& other) { this->AssingnRef(other); return *this; }
	};

	// ========================================================================================================================

	/**
	A bitmap resource that contains a raw bitmap. This ig enerated by the generated, and loaded into the texture object.
	*/
	class BitmapResource : public Referencable {
	public:
		BitmapResource() : m_bmsize(0), m_ch(0), m_x(0), m_y(0), m_data(nullptr), m_stride(0) {}
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

		operator ID3D11Texture2D* () { return this->m_pTex; }
		operator ID3D11ShaderResourceView* () { return this->m_pResourceView; }
		operator ID3D11RenderTargetView* () { return this->m_pRenderTargetView; }

		void SetRenderTargetView(Renderer & device, size_t id =0);

	private:
		ID3D11Texture2D * m_pTex;
		ID3D11ShaderResourceView* m_pResourceView;
		ID3D11RenderTargetView* m_pRenderTargetView;
	};

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

	typedef Ref<TextureSampler> TextureSamplerRef_t;
	typedef TextureSamplerRef_t TextureSamplerRef;
}

#define EX_ERROR_TEXTURE 1200

DEFINE_EXCEPTION(TextureCreateException, EX_ERROR_TEXTURE + 1, "Could not create texture")
DEFINE_EXCEPTION(ShaderResourceViewException, EX_ERROR_TEXTURE + 2, "Could not create shader resource view")
DEFINE_EXCEPTION(RenderTargetViewException, EX_ERROR_TEXTURE + 3, "Could not create render target view")
DEFINE_EXCEPTION(NoRenderTargetViewException, EX_ERROR_TEXTURE + 4, "Texture was not created as bare, and has no render target view")
DEFINE_EXCEPTION(SamplerStateCreateException, EX_ERROR_TEXTURE + 5, "Could not create sampler state")
