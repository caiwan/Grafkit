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
		throw EX(NoRenderTargetViewException);
	}

	device.SetRenderTargetView(m_pTargetView, id);
}

// TODO: fuct this crapshit out of all over the place hard
void Grafkit::ATexture::CrateTexture(Renderer & device, DXGI_FORMAT format, int channels, int chw, int w, int h, int d, bool isDynamic, bool hasMips, bool cubemap)
{
	HRESULT result;

	m_ch = channels;
	m_chW = chw;
	m_w = w;
	m_h = h;
	m_d = d;

	int dimension = GetDimension();

	m_format = format;

	D3D11_SRV_DIMENSION srvDimension;
	D3D11_RTV_DIMENSION rtvDimension;

	D3D11_USAGE usage = isDynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
	UINT bindFlags = (!isDynamic ? D3D11_BIND_RENDER_TARGET : 0) | D3D11_BIND_SHADER_RESOURCE;
	UINT CPUAccessFlags = isDynamic ? D3D11_CPU_ACCESS_WRITE : 0;
	UINT miscFlags = 0;

	switch (dimension)
	{
		// Create 1D Texture
	case 1:
	{
		if (!m_w)
			return;

		srvDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
		rtvDimension = D3D11_RTV_DIMENSION_TEXTURE1D;

		// ... 
		D3D11_TEXTURE1D_DESC textureDesc;
		ZeroMemory(&textureDesc, sizeof(textureDesc));
		textureDesc.Width = m_w;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = m_format;
		textureDesc.Usage = usage;
		textureDesc.BindFlags = bindFlags;
		textureDesc.CPUAccessFlags = CPUAccessFlags;
		textureDesc.MiscFlags = miscFlags;

		ID3D11Texture1D *ppTex = nullptr;
		result = device->CreateTexture1D(&textureDesc, nullptr, &ppTex);

		if (FAILED(result)) throw EX(TextureCreateException);

		m_pTexture = ppTex;

	} break;
	// Create 2D Texture
	case 2:
	{
		if (!m_w && !m_h) {
			int sw = 0, sh = 0;
			device.GetScreenSize(sw, sh);
			m_w = sw, m_h = sh;
		}
		else if (!m_w || !m_h) {
			return;
		}

		srvDimension = !cubemap ? D3D11_SRV_DIMENSION_TEXTURE2D : D3D11_SRV_DIMENSION_TEXTURECUBE;
		rtvDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

		D3D11_TEXTURE2D_DESC textureDesc;
		ZeroMemory(&textureDesc, sizeof(textureDesc));
		textureDesc.Width = m_w;
		textureDesc.Height = m_h;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = cubemap ? 6 : 1;
		textureDesc.Format = m_format;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Usage = usage;
		textureDesc.BindFlags = bindFlags;
		textureDesc.CPUAccessFlags = CPUAccessFlags;
		textureDesc.MiscFlags = miscFlags | cubemap ? D3D11_RESOURCE_MISC_TEXTURECUBE : 0;

		ID3D11Texture2D *ppTex = nullptr;
		result = device->CreateTexture2D(&textureDesc, nullptr, &ppTex);

		if (FAILED(result)) throw EX(TextureCreateException);

		m_pTexture = ppTex;
	} break;
	// Create 3D Texture
	case 3:
	{
		if (!m_w || !m_h || !m_d)
			return;

		srvDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
		rtvDimension = D3D11_RTV_DIMENSION_TEXTURE3D;

		// ... if needed
	} break;
	default:
		return;
	}

	// --- 
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	ZeroMemory(&shaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	shaderResourceViewDesc.Format = m_format;
	shaderResourceViewDesc.ViewDimension = srvDimension;

	switch (GetDimension()) {
	case 1:
		shaderResourceViewDesc.Texture1D.MostDetailedMip = 0;
		shaderResourceViewDesc.Texture1D.MipLevels = 1;
		break;
	case 2:
		if (!cubemap) {
			shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
			shaderResourceViewDesc.Texture2D.MipLevels = 1;
		}
		else {
			shaderResourceViewDesc.TextureCube.MostDetailedMip = 0;
			shaderResourceViewDesc.TextureCube.MipLevels = 1;
		}
		break;
	case 3:
		shaderResourceViewDesc.Texture3D.MostDetailedMip = 0;
		shaderResourceViewDesc.Texture3D.MipLevels = 1;
		break;
	}

	result = device->CreateShaderResourceView(m_pTexture, &shaderResourceViewDesc, &m_pResourceView);

	if (FAILED(result))
		throw EX(ShaderResourceViewException);

	// --- 
	if (!isDynamic && !cubemap) {
		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
		ZeroMemory(&renderTargetViewDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
		renderTargetViewDesc.Format = m_format;
		renderTargetViewDesc.ViewDimension = rtvDimension;

		switch (GetDimension()) {
		case 1:
			renderTargetViewDesc.Texture1D.MipSlice = 0;
			break;
		case 2:
			renderTargetViewDesc.Texture2D.MipSlice = 0;
			break;
		case 3:
			renderTargetViewDesc.Texture3D.MipSlice = 0;
			break;
		}

		result = device->CreateRenderTargetView(m_pTexture, &renderTargetViewDesc, &m_pTargetView);

		if (FAILED(result))
			throw EX(RenderTargetViewException);
	}
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

void Grafkit::ATexture::Channel3To4(const void * in, void * out, size_t w, size_t h)
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

		Channel3To4(data, subRes.pData, w, h);

		device.GetDeviceContext()->Unmap(m_pTexture, NULL);
	}
	else
	{
		UpdateTexture(device, data, w * h * ch * m_chW, index);
	}

}

//void Grafkit::ATexture::Update(Renderer & device, const CubemapRef cubemap)
//{
//	// https://msdn.microsoft.com/en-us/library/windows/desktop/bb204881(v=vs.85).aspx
//	// https://msdn.microsoft.com/en-us/library/windows/desktop/ff476906(v=vs.85).aspx
//	Update(device, cubemap->GetPosX(), 0);
//	Update(device, cubemap->GetNegX(), 1);
//	Update(device, cubemap->GetPosY(), 2);
//	Update(device, cubemap->GetNegY(), 3);
//	Update(device, cubemap->GetPosZ(), 4);
//	Update(device, cubemap->GetNegZ(), 5);
//}

// ========================================================================================================================

void Grafkit::Texture1D::Initialize(Renderer & device, size_t w, const float * data)
{
	this->CrateTexture(device, DXGI_FORMAT_R32_FLOAT, 1, 4, w);
}


// ========================================================================================================================

void Grafkit::Texture2D::Initialize(Renderer & device, BitmapRef bitmap)
{
	DXGI_FORMAT fmt;
	int ch = bitmap->GetCh();
	int chw = 1;

	switch (ch) {
	case 1:
		fmt = DXGI_FORMAT_R8_UNORM;
		break;
	case 2:
		fmt = DXGI_FORMAT_R8G8_UNORM;
		break;
	case 3:
	case 4:
#ifdef USE_SRGB_TEXTURE
		fmt = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
#else
		fmt = DXGI_FORMAT_R8G8B8A8_UNORM;
#endif
		ch = 4;
		break;
	}

	this->CrateTexture(device, fmt, ch, chw, bitmap->GetX(), bitmap->GetY());
	this->Update(device, bitmap);
}

void Grafkit::Texture2D::Initialize(Renderer & device, int w, int h)
{
	DXGI_FORMAT fmt;
#ifdef USE_SRGB_TEXTURE
	fmt = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
#else
	fmt = DXGI_FORMAT_R8G8B8A8_UNORM;
#endif
	this->CrateTexture(device, fmt, 4, 1, w, h, 0, false, false);
}

void Grafkit::Texture2D::InitializeFloatRGBA(Renderer & device, int w, int h)
{
	this->CrateTexture(device, DXGI_FORMAT_R32G32B32A32_FLOAT, 4, 4, w, h, 0, false, false);
}

void Grafkit::Texture2D::InitializeFloat(Renderer & device, int w, int h)
{
	this->CrateTexture(device, DXGI_FORMAT_R32_FLOAT, 4, 1, w, h, 0, false, false);
}

void Grafkit::Texture2D::InitializeDepth(Renderer & device, int w, int h)
{
	this->CrateTexture(device, DXGI_FORMAT_D32_FLOAT, 4, 1, w, h, 0, false, false);
}

// ========================================================================================================================

void Grafkit::TextureCube::Initialize(Renderer device, CubemapRef cubemap)
{

	HRESULT result;

	bool isConvertCh3to4 = false;

	DXGI_FORMAT fmt;
	BitmapRef bitmap = cubemap->GetPosX(); //assume that everything is equal
	int ch = bitmap->GetCh();
	int chw = 1;

	switch (ch) {
	case 1:
		fmt = DXGI_FORMAT_R8_UNORM;
		break;
	case 2:
		fmt = DXGI_FORMAT_R8G8_UNORM;
		break;
	case 3:
		isConvertCh3to4 = true;
	case 4:
#ifdef USE_SRGB_TEXTURE
		fmt = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
#else
		fmt = DXGI_FORMAT_R8G8B8A8_UNORM;
#endif
		ch = 4;
		break;
	}

	m_format = fmt;

	m_w = cubemap->GetPosX()->GetX();
	m_h = cubemap->GetPosX()->GetY();
	m_ch = ch;
	m_chW = 1;


	//this->CrateTexture(device, fmt, ch, chw, bitmap->GetX(), bitmap->GetY(), 0, true, true, true);
	//this->Update(device, cubemap);

	D3D11_SRV_DIMENSION srvDimension;
	D3D11_RTV_DIMENSION rtvDimension;

	srvDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	rtvDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));
	textureDesc.Width = m_w;
	textureDesc.Height = m_h;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 6;
	textureDesc.Format = m_format;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	// setup inital data 
	D3D11_SUBRESOURCE_DATA pData[6];

	for (int i = 0; i < 6; i++)
	{
		void *inData = cubemap->GetBitmap(i)->GetData();
		void *outData = inData;
		if (isConvertCh3to4) {
			outData = new unsigned char[m_w * m_h * m_ch * m_chW];
			Channel3To4(inData, outData, m_w, m_h);
		}
		pData[i].pSysMem = outData;
		pData[i].SysMemPitch = m_w * 4;
		pData[i].SysMemSlicePitch = 0;
	}

	ID3D11Texture2D *ppTex = nullptr;
	result = device->CreateTexture2D(&textureDesc, pData, &ppTex);

	if (isConvertCh3to4)
		for (int i = 0; i < 6; i++)
			delete pData[i].pSysMem;

	if (FAILED(result)) throw EX(TextureCreateException);

	m_pTexture = ppTex;

	// set srv
	// --- 
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	ZeroMemory(&shaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	shaderResourceViewDesc.Format = m_format;
	shaderResourceViewDesc.ViewDimension = srvDimension;


	shaderResourceViewDesc.TextureCube.MostDetailedMip = 0;
	shaderResourceViewDesc.TextureCube.MipLevels = 1;

	result = device->CreateShaderResourceView(m_pTexture, &shaderResourceViewDesc, &m_pResourceView);
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


