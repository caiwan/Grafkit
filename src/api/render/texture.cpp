#include "stdafx.h"

#include "Texture.h"

using namespace std;
using namespace Grafkit;
using namespace Grafkit;

using namespace FWdebugExceptions;

//using Grafkit::Matrix;

using namespace DirectX;
using namespace Grafkit;

#if 0
#define TEX_FMT_1CH DXGI_FORMAT_R8_UNORM
#define TEX_FMT_2CH DXGI_FORMAT_R8G0_UNORM
#define TEX_FMT_4CH DXGI_FORMAT_R8G8B8A8_UNORM
#else
#define TEX_FMT_1CH DXGI_FORMAT_R8_UNORM
#define TEX_FMT_2CH DXGI_FORMAT_R8G8_UNORM
#define TEX_FMT_4CH DXGI_FORMAT_R8G8B8A8_UNORM_SRGB
#endif

// ========================================================================================================================
// a bit of badly hacked thing, might need to clean it up later on
// but for a while, it does well. 
// ========================================================================================================================

Grafkit::ATexture::ATexture() : 
	m_pResourceView(nullptr), m_pTargetView(nullptr), m_pTexture(nullptr),
	m_mipSlices(0), m_mipLevels(0), 
	m_w(0), m_h(0), m_d(0)
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

void Grafkit::ATexture::CreateTexture(Renderer & device, DXGI_FORMAT format, int w, int h, int d)
{
	HRESULT result;

	m_w = w;
	m_h = h;
	m_d = d;

	int dimension = GetDimension();

	m_format = format;

	D3D11_SRV_DIMENSION srvDimension;
	D3D11_RTV_DIMENSION rtvDimension;

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
		}
		// Create 2D Texture
		case 2:
		{
			if (!m_w && !m_h)
				device.GetScreenSize(m_w, m_h);
			else if (!m_w || !m_h)
				return;

			srvDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			rtvDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

			D3D11_TEXTURE2D_DESC textureDesc;
			ZeroMemory(&textureDesc, sizeof(textureDesc));
			textureDesc.Width = m_w;
			textureDesc.Height = m_h;
			textureDesc.MipLevels = 1;
			textureDesc.ArraySize = 1;
			textureDesc.Format = m_format;
			textureDesc.SampleDesc.Count = 1;
			textureDesc.Usage = D3D11_USAGE_DEFAULT;
			textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
			textureDesc.CPUAccessFlags = 0;
			textureDesc.MiscFlags = 0;

			ID3D11Texture2D *ppTex = nullptr;
			result = device->CreateTexture2D(&textureDesc, nullptr, &ppTex);

			if (FAILED(result)) throw EX(TextureCreateException);
			
			m_pTexture = ppTex;
		}
		// Create 3D Texture
		case 3:
		{
			if (!m_w || !m_h || !m_d)
				return;

			srvDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
			rtvDimension = D3D11_RTV_DIMENSION_TEXTURE3D;

			// ... if needed
		}
		default:
			return;
	}

	// --- 
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	ZeroMemory(&shaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	shaderResourceViewDesc.Format = m_format;
	shaderResourceViewDesc.ViewDimension = srvDimension;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	result = device->CreateShaderResourceView(m_pTexture, &shaderResourceViewDesc, &m_pResourceView);

	if (FAILED(result))
		throw EX(ShaderResourceViewException);

	// --- 
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	ZeroMemory(&renderTargetViewDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
	renderTargetViewDesc.Format = m_format;
	renderTargetViewDesc.ViewDimension = rtvDimension;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	result = device->CreateRenderTargetView(m_pTexture, &renderTargetViewDesc, &m_pTargetView);

	if (FAILED(result))
		throw EX(RenderTargetViewException);

}

void Grafkit::ATexture::UpdateTexture(Renderer & device, const void * data, size_t len)
{
	D3D11_MAPPED_SUBRESOURCE subRes;
	device.GetDeviceContext()->Map(m_pTexture, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &subRes);
	CopyMemory(subRes.pData, data, len);
	device.GetDeviceContext()->Unmap(m_pTexture, NULL);
}

void Grafkit::ATexture::UpdateTexture(Renderer & device, const BitmapRef bitmap)
{
	UINT w = bitmap->GetX(), h = bitmap->GetY(), ch = bitmap->GetCh();
	UCHAR* data = (UCHAR*)bitmap->GetData();

	if (ch == 3)
	{
		UINT* newdata = new UINT[w*h];
		UCHAR* dst = (UCHAR*)newdata;
		UCHAR* src = (UCHAR*)data;

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

		ch = 4;

		data = (UCHAR*)newdata;
	} 

	UpdateTexture(device, data, w * h * ch * sizeof(UCHAR));

	if (data != bitmap->GetData())
		delete[] data;
}

// ========================================================================================================================

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


