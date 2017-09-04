#include "stdafx.h"

#include "Texture.h"

using namespace std;
using namespace Grafkit;
using namespace Grafkit;

using namespace FWdebugExceptions;

//using Grafkit::Matrix;

using namespace DirectX;
using namespace Grafkit;

#define USE_SRGB_TEXTURE

namespace {
	DXGI_FORMAT bitmapformats[] = {
		DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_R8_UNORM,
		DXGI_FORMAT_R8G8_UNORM,
#ifdef USE_SRGB_TEXTURE
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
#else
		DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_FORMAT_R8G8B8A8_UNORM,
#endif
	};
}

// ========================================================================================================================
// a bit of badly hacked thing, might need to clean it up later on
// but for a while, it does well. 
// ========================================================================================================================

Grafkit::ATexture::ATexture() :
	m_pResourceView(nullptr), m_pTargetView(nullptr), m_pTexture(nullptr),
	//m_mipSlices(0), m_mipLevels(0), 
	m_w(0), m_h(0), m_d(0),
	m_ch(0), m_chW(0)
{
}

Grafkit::ATexture::~ATexture()
{
	Shutdown();
}

void Grafkit::ATexture::Shutdown()
{
	if (m_pResourceView)
	{
		m_pResourceView->Release();
		m_pResourceView = nullptr;
	}

	if (m_pTargetView)
	{
		m_pTargetView->Release();
		m_pTargetView = nullptr;
	}

	if (m_pTexture)
	{
		m_pTexture->Release();
		m_pTexture = nullptr;
	}
}

void Grafkit::ATexture::SetRenderTargetView(Renderer & device, size_t id) const
{
	if (!m_pTargetView) {
		throw new EX(NoRenderTargetViewException);
	}

	device.SetRenderTargetView(m_pTargetView, id);
}

void Grafkit::ATexture::UpdateTexture(Renderer & device, const void * data, size_t len, size_t subRes, size_t offset)
{
	if (!len || !data)
		return;

	D3D11_MAPPED_SUBRESOURCE res;
	device.GetDeviceContext()->Map(m_pTexture, subRes, D3D11_MAP_WRITE_DISCARD, NULL, &res);
	CopyMemory((unsigned char*)(res.pData) + offset, data, len);
	device.GetDeviceContext()->Unmap(m_pTexture, subRes);
}

void Grafkit::ATexture::CopyChannel3To4(const void * in, void * out, size_t w, size_t h)
{
	UCHAR* dst = (UCHAR*)out;
	UCHAR* src = (UCHAR*)in;

	size_t x = w;
	size_t y = h;

	size_t stride_src = x * 3;
	size_t stride_dst = x * 4;

	while (y--) {
		while (x--) {
			dst[4 * x + 0] = src[3 * x + 0];
			dst[4 * x + 1] = src[3 * x + 1];
			dst[4 * x + 2] = src[3 * x + 2];
			dst[4 * x + 3] = 0xff;
		}
		src += stride_src;
		dst += stride_dst;
		x = w;
	}
}

void Grafkit::ATexture::Update(Renderer & device, const void * bitmap, size_t index)
{
	size_t len = 0;
	switch (GetDimension()) {
	case 1:	len = m_ch * m_chW * m_w; break;
	case 2:	len = m_ch * m_chW * m_w * m_h; break;
	case 3: len = m_ch * m_chW * m_w * m_h * m_d; break;
	default:  return;
	}

	UpdateTexture(device, bitmap, len, index);
}

void Grafkit::ATexture::Update(Renderer & device, const BitmapRef bitmap, size_t index)
{
	UINT w = bitmap->GetX(), h = bitmap->GetY(), ch = bitmap->GetCh();
	UCHAR* data = (UCHAR*)bitmap->GetData();

	if (ch == 3)
	{
		D3D11_MAPPED_SUBRESOURCE subRes;
		subRes.pData = nullptr;
		subRes.DepthPitch = 0;
		subRes.RowPitch = 0;
		device.GetDeviceContext()->Map(m_pTexture, index, D3D11_MAP_WRITE_DISCARD, NULL, &subRes);

		CopyChannel3To4(data, subRes.pData, w, h);

		device.GetDeviceContext()->Unmap(m_pTexture, NULL);
	}
	else
	{
		UpdateTexture(device, data, w * h * ch * m_chW, index);
	}

}

// ========================================================================================================================

void Grafkit::Texture1D::Initialize(Renderer & device, size_t w, const float * data)
{
	m_w = w;
	m_h = 0;
	m_d = 0;
	m_ch = 1;
	m_chW = 4;
	this->CreateTexture(device, DXGI_FORMAT_R32_FLOAT, w, data);
}

void Grafkit::Texture1D::CreateTexture(Renderer & device, DXGI_FORMAT format, size_t w, const void * initaldata)
{
	HRESULT result = 0;

	D3D11_SRV_DIMENSION srvDimension = D3D11_SRV_DIMENSION_TEXTURE1D;

	// ... 
	D3D11_TEXTURE1D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));
	textureDesc.Width = m_w;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = m_format;
	textureDesc.Usage = D3D11_USAGE_DYNAMIC;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	D3D11_SUBRESOURCE_DATA *pData = nullptr, subresData;
	// initialdata
	if (initaldata) {
		// ... 
		subresData.pSysMem = initaldata;
		subresData.SysMemPitch = m_w * m_chW;
		subresData.SysMemSlicePitch = 0;
		pData = &subresData;
	}

	ID3D11Texture1D *ppTex = nullptr;
	result = device->CreateTexture1D(&textureDesc, pData, &ppTex);

	if (FAILED(result)) throw new EX(TextureCreateException);

	m_pTexture = ppTex;

	// SRV

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	ZeroMemory(&shaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	shaderResourceViewDesc.Format = m_format;
	shaderResourceViewDesc.ViewDimension = srvDimension;

	shaderResourceViewDesc.Texture1D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture1D.MipLevels = -1;

	result = device->CreateShaderResourceView(m_pTexture, &shaderResourceViewDesc, &m_pResourceView);
}



// ========================================================================================================================

void Grafkit::Texture2D::Initialize(Renderer & device, BitmapRef bitmap)
{
	DXGI_FORMAT fmt;
	int ch = bitmap->GetCh();
	int chw = 1;

	fmt = bitmapformats[ch % 5];

	void * data = bitmap->GetData();

	CreateTextureBitmap(device, fmt, ch, bitmap->GetX(), bitmap->GetY(), data);
}

void Grafkit::Texture2D::Initialize(Renderer & device, int w, int h)
{
	DXGI_FORMAT fmt;
#ifdef USE_SRGB_TEXTURE
	fmt = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
#else
	fmt = DXGI_FORMAT_R8G8B8A8_UNORM;
#endif
	//this->CrateTexture(device, fmt, 4, 1, w, h, 0, false, false);
	CreateTextureTarget(device, fmt, 4, 1, w, h);
}

void Grafkit::Texture2D::InitializeFloatRGBA(Renderer & device, int w, int h)
{
	CreateTextureTarget(device, DXGI_FORMAT_R32G32B32A32_FLOAT, 4, 4, w, h);
}

void Grafkit::Texture2D::InitializeFloat(Renderer & device, int w, int h)
{
	CreateTextureTarget(device, DXGI_FORMAT_R32_FLOAT, 4, 1, w, h);
}

void Grafkit::Texture2D::InitializeDepth(Renderer & device, int w, int h)
{
	CreateTextureTarget(device, DXGI_FORMAT_D32_FLOAT, 4, 1, w, h);
}

void Grafkit::Texture2D::CreateTextureBitmap(Renderer & device, DXGI_FORMAT format, int channels, int w, int h, const void * initialData)
{
	HRESULT result;

	m_ch = channels;
	m_chW = 1;
	m_w = w;
	m_h = h;
	m_d = 0;

	m_format = format;

	if (!m_w && !m_h) {
		int sw = 0, sh = 0;
		device.GetScreenSize(sw, sh);
		m_w = sw, m_h = sh;
	}
	else if (!m_w || !m_h) {
		return;
	}

	D3D11_SRV_DIMENSION srvDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	D3D11_RTV_DIMENSION rtvDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));
	textureDesc.Width = m_w;
	textureDesc.Height = m_h;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = m_format;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	D3D11_SUBRESOURCE_DATA *pData = nullptr, subresData;

	// Initial data
	if (initialData) {
		subresData.pSysMem = initialData;
		subresData.SysMemSlicePitch = 0;
		if (m_ch == 3) {
			m_ch = 4;
			unsigned char *data = new unsigned char[m_w * m_h * m_ch * m_chW];
			CopyChannel3To4(initialData, data, m_w, m_h);
			subresData.pSysMem = data;
		}
		subresData.SysMemPitch = m_w * m_ch;
		pData = &subresData;
	}

	ID3D11Texture2D *ppTex = nullptr;
	result = device->CreateTexture2D(&textureDesc, pData, &ppTex);

	if (FAILED(result)) throw new EX(TextureCreateException);

	m_pTexture = ppTex;

	if (pData->pSysMem != initialData)
		delete pData->pSysMem;

	// SRV, RTV
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	ZeroMemory(&shaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	shaderResourceViewDesc.Format = m_format;
	shaderResourceViewDesc.ViewDimension = srvDimension;

	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = -1;

	result = device->CreateShaderResourceView(m_pTexture, &shaderResourceViewDesc, &m_pResourceView);

	if (FAILED(result))
		throw new EX(ShaderResourceViewException);
}

void Grafkit::Texture2D::CreateTextureTarget(Renderer & device, DXGI_FORMAT format, int channels, int channelWidth, int w, int h)
{
	HRESULT result;

	m_ch = channels;
	m_chW = 1;
	m_w = w;
	m_h = h;
	m_d = 0;

	m_format = format;

	if (!m_w && !m_h) {
		int sw = 0, sh = 0;
		device.GetScreenSize(sw, sh);
		m_w = sw, m_h = sh;
	}
	else if (!m_w || !m_h) {
		return;
	}

	D3D11_SRV_DIMENSION srvDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	D3D11_RTV_DIMENSION rtvDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));
	textureDesc.Width = m_w;
	textureDesc.Height = m_h;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = m_format;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA *pData = nullptr, subresData;

	ID3D11Texture2D *ppTex = nullptr;
	result = device->CreateTexture2D(&textureDesc, nullptr, &ppTex);

	if (FAILED(result)) throw new EX(TextureCreateException);

	m_pTexture = ppTex;

	// SRV
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	ZeroMemory(&shaderResourceViewDesc, sizeof(shaderResourceViewDesc));
	shaderResourceViewDesc.Format = m_format;
	shaderResourceViewDesc.ViewDimension = srvDimension;

	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = -1;

	result = device->CreateShaderResourceView(m_pTexture, &shaderResourceViewDesc, &m_pResourceView);

	if (FAILED(result))
		throw new EX(ShaderResourceViewException);

	// RTV
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	ZeroMemory(&renderTargetViewDesc, sizeof(renderTargetViewDesc));
	renderTargetViewDesc.Format = m_format;
	renderTargetViewDesc.ViewDimension = rtvDimension;

	renderTargetViewDesc.Texture2D.MipSlice = 0;

	result = device->CreateRenderTargetView(m_pTexture, &renderTargetViewDesc, &m_pTargetView);

	if (FAILED(result))
		throw new EX(RenderTargetViewException);
}


// ========================================================================================================================

void Grafkit::TextureCube::Initialize(Renderer & device, CubemapRef cubemap)
{

	HRESULT result;

	BitmapRef bitmap = cubemap->GetPosX(); //assume that everything is equal there
	int ch = bitmap->GetCh();
	DXGI_FORMAT fmt = bitmapformats[ch % 5];

	m_format = fmt;

	m_w = cubemap->GetPosX()->GetX();
	m_h = cubemap->GetPosX()->GetY();
	m_ch = (m_ch == 3) ? 4 : ch; // if bitmap has 3 component per pixel, we'll force it to 4 and convert it before creating initial data
	m_chW = 1;

	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));
	textureDesc.Width = m_w;
	textureDesc.Height = m_h;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 6;
	textureDesc.Format = m_format;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE| D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE | D3D11_RESOURCE_MISC_GENERATE_MIPS;

	// --- setup inital data 
	D3D11_SUBRESOURCE_DATA pData[6];

	for (int i = 0; i < 6; i++)
	{
		void *inData = cubemap->GetBitmap(i)->GetData();
		void *outData = inData;
		if (cubemap->GetBitmap(i)->GetCh() == 3) {
			outData = new unsigned int[m_w * m_h];
			CopyChannel3To4(inData, outData, m_w, m_h);
		}
		pData[i].pSysMem = outData;
		pData[i].SysMemPitch = m_w * 4;
		pData[i].SysMemSlicePitch = 0;
	}

	ID3D11Texture2D *ppTex = nullptr;
	result = device->CreateTexture2D(&textureDesc, pData, &ppTex);

	// cleanup converted data 
	for (int i = 0; i < 6; i++)
		if (pData[i].pSysMem != cubemap->GetBitmap(i)->GetData())
			delete pData[i].pSysMem;

	if (FAILED(result)) 
		throw new EX_HRESULT(TextureCreateException, result);

	m_pTexture = ppTex;

	// --- srv
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	ZeroMemory(&shaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	shaderResourceViewDesc.Format = m_format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;

	shaderResourceViewDesc.TextureCube.MostDetailedMip = 0;
	shaderResourceViewDesc.TextureCube.MipLevels = -1;

	result = device->CreateShaderResourceView(m_pTexture, &shaderResourceViewDesc, &m_pResourceView);

	if (FAILED(result)) throw new EX_HRESULT(TextureCreateException, result);

	device.GetDeviceContext()->GenerateMips(m_pResourceView);
}

// ========================================================================================================================

Grafkit::TextureSampler::TextureSampler()
	: m_pSamplerState(nullptr)
{
}

Grafkit::TextureSampler::~TextureSampler()
{
	this->Shutdown();
}

void Grafkit::TextureSampler::Initialize(Renderer & device)
{
	HRESULT result = 0;

	// Create a texture sampler state description.
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	//samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	result = device->CreateSamplerState(&samplerDesc, &m_pSamplerState);
	if (FAILED(result))
	{
		throw new EX(SamplerStateCreateException);
	}
}

void Grafkit::TextureSampler::Shutdown()
{
	if (m_pSamplerState)
	{
		m_pSamplerState->Release();
		m_pSamplerState = nullptr;
	}
}


