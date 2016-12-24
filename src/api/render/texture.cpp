#include "stdafx.h"

#include "Texture.h"

using namespace std;
using namespace Grafkit;
using namespace Grafkit;

using namespace FWdebugExceptions;

//using Grafkit::Matrix;

using namespace DirectX;
using namespace Grafkit;

Texture::Texture() //: IResource(),
	:
	m_pTex(nullptr),
	m_pResourceView(nullptr),
	m_pRenderTargetView(nullptr)
{
}

Texture::~Texture()
{
	this->Shutdown();
}

/// @todo a kozos struct kitoltos reszeket ki kell tenni valami kozos helyre 

void Grafkit::Texture::Initialize(Renderer & device, BitmapResourceRef bitmap)
{
	UINT x = bitmap->GetX(), y = bitmap->GetY(), ch = bitmap->GetCh();

	HRESULT result = 0;

	// fill etexture description
	D3D11_TEXTURE2D_DESC desc; ZeroMemory(&desc, sizeof(desc));

	UCHAR* data = (UCHAR*)bitmap->GetData();

	desc.Width = x;
	desc.Height = y;
	desc.MipLevels = 1; // 0 = general magatol mipmapet
	desc.ArraySize = 1; // multitextura szama, cubemap eseten n*6

	/// @todo: ezt a structlibbol kellene potolni
	switch (ch) {
		case 1: desc.Format = DXGI_FORMAT_R8_UNORM; break;
		case 2: desc.Format = DXGI_FORMAT_R8G8_UNORM; break;
		case 3:
		{
			UINT* newdata = new UINT[x*y];
			UCHAR* dst = (UCHAR*)newdata;
			UCHAR* src = (UCHAR*)data;

			size_t wx = x;
			size_t wy = y;

			size_t stride_src = wx * 3;
			size_t stride_dst = wx * 4;

			while (wy--) {
				while (wx--) {
					dst[4 * wx + 0] = src[3 * wx + 0];
					dst[4 * wx + 1] = src[3 * wx + 1];
					dst[4 * wx + 2] = src[3 * wx + 2];
					dst[4 * wx + 3] = 0xff;
				}
				src += stride_src;
				dst += stride_dst;
				wx = x;
			}

			ch = 4;

			data = (UCHAR*)newdata;
			desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		} break;

		case 4: desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; break;

		default: 
			return ;
	}

	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

#define N 1
	D3D11_SUBRESOURCE_DATA *initialData = new D3D11_SUBRESOURCE_DATA[N];
	ZeroMemory(initialData, N * sizeof(initialData));
	for (size_t i = 0; i < N; i++)
	{
		initialData[i].pSysMem = data;
		initialData[i].SysMemPitch = x * ch;
		initialData[i].SysMemSlicePitch = 0;

	}
#undef N

	result = device->CreateTexture2D(&desc, initialData, &m_pTex);

	if (FAILED(result)) {
		delete[] initialData;
		throw EX(TextureCreateException);
	}

	if (data != bitmap->GetData()) 
		delete[] data;
	delete[] initialData;

	// --- shderresourceref
	D3D11_SHADER_RESOURCE_VIEW_DESC resDesc;
	ZeroMemory(&resDesc, sizeof(resDesc));

	resDesc.Format = desc.Format;
	resDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	resDesc.Texture2D.MostDetailedMip = 0;
	resDesc.Texture2D.MipLevels = 1;

	result = device->CreateShaderResourceView(&m_pTex[0], &resDesc, &m_pResourceView);
	if (FAILED(result))
	{
		throw EX(ShaderResourceViewException);
	}
}

void Grafkit::Texture::Initialize(Renderer & device, size_t w, size_t h)
{
	D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT result;
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

	int screenW = 0, ScreenH = 0; device.GetScreenSize(screenW, ScreenH);

	// Initialize the render target texture description.
	ZeroMemory(&textureDesc, sizeof(textureDesc));

	// Setup the render target texture description.
	textureDesc.Width = w ? w : screenW;
	textureDesc.Height = h ? h : ScreenH;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	// Create the render target texture.
	result = device->CreateTexture2D(&textureDesc, nullptr, &m_pTex);
	if (FAILED(result))
	{
		throw EX(TextureCreateException);
	}

	// Setup the description of the render target view.
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// Create the render target view.
	result = device->CreateRenderTargetView(m_pTex, &renderTargetViewDesc, &m_pRenderTargetView);
	if (FAILED(result))
	{
		throw EX(RenderTargetViewException);
	}

	// Setup the description of the shader resource view.
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	// Create the shader resource view.
	result = device->CreateShaderResourceView(m_pTex, &shaderResourceViewDesc, &m_pResourceView);
	if (FAILED(result))
	{
		throw EX(ShaderResourceViewException);
	}
}

void Texture::Shutdown()
{
	if (m_pResourceView) 
	{
		m_pResourceView->Release();
		m_pResourceView = nullptr;
	}

	if (m_pRenderTargetView)
	{
		m_pRenderTargetView->Release(); // ez valamiert nullptr exceptiont dob 
		m_pRenderTargetView = nullptr;
	}

	if (m_pTex)
	{
		m_pTex->Release();
		m_pTex = nullptr;
	}


}

void Grafkit::Texture::SetRenderTargetView(Renderer & device, size_t id)
{
	if (!m_pRenderTargetView) {
		throw EX(NoRenderTargetViewException);
	}

	device.SetRenderTargetView(m_pRenderTargetView, id);
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
