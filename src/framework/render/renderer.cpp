#include "stdafx.h"

#include "Renderer.h"
#include "core/memory.h"

using namespace FWdebugExceptions;
using namespace Grafkit;

Renderer::Renderer() :
	m_swapChain(nullptr),
	m_device(nullptr),
	m_deviceContext(nullptr),
	m_renderTargetViewCount(0), m_myRenderTargetView(nullptr),
	m_depthStencilBuffer(nullptr),
	m_depthStencilState(nullptr), m_depthStencilStateWriteDisabled(nullptr),
	m_myDepthStencilView(nullptr),
	m_depthStencilView(nullptr),
	m_rasterState(nullptr),
	m_screenW(0),
	m_screenH(0),
	m_lastDeltaTime(0.f)
{
	m_lastTimePoint = std::chrono::system_clock::now();
}

Renderer::~Renderer()
{
	this->Shutdown();
}


int Renderer::Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, float aspectw, float aspecth)
{
	HRESULT result;
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	size_t stringLength = 0;
	UINT numModes = 0, i = 0, numerator = 0, denominator = 0;
	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;
	int error;
	D3D_FEATURE_LEVEL featureLevel;

	m_screenW = screenWidth;
	m_screenH = screenHeight;

	// Store the vsync setting.
	m_vsync_enabled = vsync;

	// Create a DirectX graphics interface factory.
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result))
		THROW_EX_HRESULT(InitializeRendererException, result);

	// Use the factory to create an adapter for the primary graphics interface (video card).
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result))
		THROW_EX_HRESULT(InitializeRendererException, result);

	// Enumerate the primary adapter output (monitor).
	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result))
		THROW_EX_HRESULT(InitializeRendererException, result);

	// Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, nullptr);

	// Create a list to hold all the possible display modes for this monitor/video card combination.
	// ... but when using offscreen surface, like rendering, there will be no exact rendering surface to hook up to
	// so ignore errors instead

	const int noSurfaceOccured = (result == 0x887a0022);

	if (FAILED(result) && !noSurfaceOccured)
		THROW_EX_HRESULT(InitializeRendererException, result);


	displayModeList = new DXGI_MODE_DESC[numModes];

	if (!displayModeList && !noSurfaceOccured)
	{
		THROW_EX_DETAILS(InitializeRendererException, "No display mode list");
	}

	// Get the adapter (video card) description.
	result = adapter->GetDesc(&adapterDesc);
	if (FAILED(result))
		THROW_EX_HRESULT(InitializeRendererException, result);

	// Convert the name of the video card to a character array and store it.
	error = wcstombs_s(&stringLength, m_videoCardDescription, 256, adapterDesc.Description, 256);
	if (error != 0)
		THROW_EX_DETAILS(InitializeRendererException, "No wcstombs_s(...)");

	Log::Logger().Info("\nDisplay adapter: vid: %s", m_videoCardDescription);

	// Now fill the display mode list structures.
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if (FAILED(result) && !noSurfaceOccured)
		THROW_EX_HRESULT(InitializeRendererException, result);

	// Now go through all the display modes and find the one that matches the screen width and height.
	// When a match is found store the numerator and denominator of the refresh rate for that monitor.
	Log::Logger().Info("\nAvailable display modes:");
	for (i = 0; i < numModes; i++)
	{
		Log::Logger().Info("%d %d x %d, %d, scale %d", i, displayModeList[i].Width, displayModeList[i].Height, displayModeList[i].RefreshRate, displayModeList[i].Scaling);
		if (displayModeList[i].Width == (unsigned int)m_screenW)
		{
			if (displayModeList[i].Height == (unsigned int)m_screenH)
			{
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	// Release the display mode list.
	delete[] displayModeList;
	displayModeList = 0;

	// Release the adapter output.
	adapterOutput->Release();
	adapterOutput = 0;

	// Release the adapter.
	adapter->Release();
	adapter = 0;

	// Release the factory.
	factory->Release();
	factory = 0;

	// -----------------------------------------------------------------------------
	// --- setup swap chain
	DXGI_SWAP_CHAIN_DESC swapChainDesc;

	// Initialize the swap chain description.
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	// Set to a single back buffer.
	swapChainDesc.BufferCount = 1;

	// Set the width and height of the back buffer.
	swapChainDesc.BufferDesc.Width = m_screenW;
	swapChainDesc.BufferDesc.Height = m_screenH;

	// Set regular 32-bit surface for the back buffer.
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	// Set the refresh rate of the back buffer.
	if (m_vsync_enabled)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	// Set the usage of the back buffer.
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// Set the handle for the window to render to.
	swapChainDesc.OutputWindow = hwnd;

	// Turn multisampling off.
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	// Set to full screen or windowed mode.
	if (fullscreen)
	{
		swapChainDesc.Windowed = false;
	}
	else
	{
		swapChainDesc.Windowed = true;
	}

	// Set the scan line ordering and scaling to unspecified.
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// Discard the back buffer contents after presenting.
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// Don't set the advanced flags.
	swapChainDesc.Flags = 0;

	// Set the feature level to DirectX 11.
	featureLevel = D3D_FEATURE_LEVEL_10_0;

	// Create the swap chain, Direct3D device, and Direct3D device context.

	DWORD deviceCreationFlags = 0;

#ifdef _DEBUG
	deviceCreationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	result = D3D11CreateDeviceAndSwapChain(
		nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, deviceCreationFlags,
		&featureLevel, 1, D3D11_SDK_VERSION, &swapChainDesc,
		&m_swapChain, &m_device, nullptr, &m_deviceContext
	);

	if (FAILED(result))
		THROW_EX_HRESULT(InitializeRendererException, result);

	// update our reference
	this->AssingnRef(m_device);

	// -----------------------------------------------------------------------------
	// setup backbuffer texture + depth buffer 

	ID3D11Texture2D* backBufferPtr;
	// Get the pointer to the back buffer.
	result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	if (FAILED(result))
		THROW_EX_HRESULT(InitializeRendererException, result);

	// -----------------------------------------------------------------------------
	// Create the render target view with the back buffer pointer.
	result = m_device->CreateRenderTargetView(backBufferPtr, nullptr, &m_myRenderTargetView);
	if (FAILED(result))
		THROW_EX_HRESULT(InitializeRendererException, result);

	// Release pointer to the back buffer as we no longer need it.
	backBufferPtr->Release();
	backBufferPtr = 0;

	// -----------------------------------------------------------------------------

	CreateDepthStencilView();

	// -----------------------------------------------------------------------------


	// Bind the render target view and depth stencil buffer to the output render pipeline.
	SetRenderTargetView();
	SetDepthStencilView();
	ApplyRenderTargetView();

	// -----------------------------------------------------------------------------

	// Create debug layer
#ifdef _DEBUG
	ID3D11Debug *d3dDebug = nullptr;
	if (SUCCEEDED(m_device->QueryInterface(__uuidof(ID3D11Debug), (void**)&d3dDebug)))
	{
		ID3D11InfoQueue *d3dInfoQueue = nullptr;
		if (SUCCEEDED(d3dDebug->QueryInterface(__uuidof(ID3D11InfoQueue), (void**)&d3dInfoQueue)))
		{

			//d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_WARNING, true);
			d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
			d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);

			D3D11_MESSAGE_ID hide[] =
			{
				D3D11_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS,
				D3D11_MESSAGE_ID_DEVICE_DRAW_SAMPLER_NOT_SET,
				D3D11_MESSAGE_ID_DEVICE_DRAW_RENDERTARGETVIEW_NOT_SET
			};

			D3D11_INFO_QUEUE_FILTER filter;
			memset(&filter, 0, sizeof(filter));
			filter.DenyList.NumIDs = _countof(hide);
			filter.DenyList.pIDList = hide;
			d3dInfoQueue->AddStorageFilterEntries(&filter);
			d3dInfoQueue->Release();
		}
		d3dDebug->Release();
	}
#endif

	// Setup the raster description which will determine how and what polygons will be drawn.
	D3D11_RASTERIZER_DESC rastDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT());
	// rastDesc.CullMode = D3D11_CULL_NONE;

	// Create the rasterizer state from the description we just filled out.
	result = m_device->CreateRasterizerState(&rastDesc, &m_rasterState);
	if (FAILED(result))
		THROW_EX_HRESULT(InitializeRendererException, result);

	// Now set the rasterizer state.
	m_deviceContext->RSSetState(m_rasterState);

	// -----------------------------------------------------------------------------
	SetViewportAspect(aspectw, aspecth);

	return true;
}

void Renderer::Shutdown()
{
	// Before shutting down set to windowed mode or when you release the swap chain it will throw an exception.
	if (m_swapChain)
	{
		m_swapChain->SetFullscreenState(false, nullptr);
	}

	RELEASE(m_rasterState);
	RELEASE(m_myDepthStencilView);
	RELEASE(m_depthStencilState);
	RELEASE(m_depthStencilBuffer);
	RELEASE(m_myRenderTargetView);
	RELEASE(m_deviceContext);
	RELEASE(m_swapChain);

	RELEASE(m_device);
	this->ptr = nullptr;

	return;
}

void Renderer::GetVideoCardInfo(char* cardName)
{
	strcpy_s(cardName, 128, m_videoCardDescription);
	return;
}

void Renderer::Resize(int screenW, int screenH) {
		HRESULT hr = 0;
	if (m_swapChain) {

		m_screenH = screenH;
		m_screenW = screenW;

		m_deviceContext->OMSetRenderTargets(0, 0, 0);
		m_myRenderTargetView->Release();


		hr = m_swapChain->ResizeBuffers(0, m_screenW, m_screenH, DXGI_FORMAT_UNKNOWN, 0);
		if (FAILED(hr))
			THROW_EX_HRESULT(ResizeRenderSurfaceException, hr);
		
		ID3D11Texture2D* pBuffer = nullptr;

		// Resize RTV
		hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBuffer);
		
		if (FAILED(hr))
			THROW_EX_HRESULT(ResizeRenderSurfaceException, hr);

		hr = m_device->CreateRenderTargetView(pBuffer, NULL, &m_myRenderTargetView);
		
		if (FAILED(hr))
			THROW_EX_HRESULT(ResizeRenderSurfaceException, hr);

		pBuffer->Release();

		// ---- 
		m_myDepthStencilView->Release();
		//m_depthStencilBuffer->Release();
		CreateDepthStencilView();
		// --- 
		
		SetRenderTargetView();
		SetDepthStencilView();

		ApplyRenderTargetView();

		SetViewport(screenW, screenH);
	}
}

void Renderer::BeginScene(float red, float green, float blue, float alpha)
{
	float color[4];


	// Setup the color to clear the buffer to.
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	// Clear the back buffer.
	for (size_t i = 0; i < this->m_renderTargetViewCount; i++)
		if (m_renderTargetViews[i])
			m_deviceContext->ClearRenderTargetView(m_renderTargetViews[i], color);

	// Clear the depth buffer.
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}

static const float black[4] = { 0.f, 0.f, 0.f, 1.f };
static const float cornflowerBlue[4] = { 0.0f, 0.2f, 0.4f, 1.0f };
/*
[19:05:47] Citrus Lee: cornflower blue :D
[19:05:52] Citrus Lee: you're welcome :D
*/

void Renderer::BeginScene()
{
	// Clear the back buffer.
	for (size_t i = 0; i < this->m_renderTargetViewCount; i++)
		if (m_renderTargetViews[i]) {
			m_deviceContext->ClearRenderTargetView(m_renderTargetViews[i], black);
		}
	// Clear the depth buffer.
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void Renderer::BeginSceneDev()
{
	// Clear the back buffer.
	for (size_t i = 0; i < this->m_renderTargetViewCount; i++)
		if (m_renderTargetViews[i]) {
			m_deviceContext->ClearRenderTargetView(m_renderTargetViews[i], cornflowerBlue);
		}
	// Clear the depth buffer.
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}


void Renderer::EndScene()
{
	// Present the back buffer to the screen since rendering is complete.
	if (m_vsync_enabled)
	{
		// Lock to screen refresh rate.
		m_swapChain->Present(1, 0);
	}
	else
	{
		// Present as fast as possible.
		m_swapChain->Present(0, 0);
	}

	auto timePoint = std::chrono::system_clock::now();
	auto deltaTime = timePoint - m_lastTimePoint;

	// Ref: http://en.cppreference.com/w/cpp/chrono/duration
	m_lastDeltaTime = float(std::chrono::duration_cast<std::chrono::microseconds>(deltaTime).count()) / 1000.f / 1000.f;
	m_lastTimePoint = timePoint;
}


void Renderer::ToggleDepthWrite(bool isEanbled)
{
	if (isEanbled)
		m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);
	else
		m_deviceContext->OMSetDepthStencilState(m_depthStencilStateWriteDisabled, 1);
}

void Renderer::SetViewport(int screenW, int screenH, int offsetX, int offsetY)
{
	// Setup the viewport for rendering.
	m_viewport.Width = float(screenW);
	m_viewport.Height = float(screenH);
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
	m_viewport.TopLeftX = float(offsetX);
	m_viewport.TopLeftY = float(offsetY);

	// TODO: the enitre render output stack and buffers has to be updated
	if (m_deviceContext)
		m_deviceContext->RSSetViewports(1, &m_viewport);
}

void Renderer::SetViewportAspect(float aspectW, float aspectH)
{
	// reset on invalid input
	if (aspectW <= 0 || aspectH <= 0)
		SetViewport(m_screenW, m_screenH, 0, 0);

	if (aspectW > aspectH) {
		float ah = aspectH / aspectW;
		float h = floor(float(m_screenH) * ah);
		float oh = floor((m_screenH - h) * .5);
		SetViewport(m_screenW, int(h), 0, int(oh));
	}
	// we dont care about the rest atm
}

void Renderer::SetRenderTargetView(ID3D11RenderTargetView * pRenderTargetView, size_t n)
{
	n = n < RENDER_TARGET_MAX ? n : RENDER_TARGET_MAX;
	if (!n && pRenderTargetView == nullptr) {
		m_renderTargetViews[0] = m_myRenderTargetView;
	}
	else {
		m_renderTargetViews[n] = pRenderTargetView;
	}
}

void Renderer::ApplyRenderTargetView(size_t count)
{
	m_renderTargetViewCount = count < RENDER_TARGET_MAX ? count : RENDER_TARGET_MAX;
	m_deviceContext->OMSetRenderTargets(m_renderTargetViewCount, m_renderTargetViews, m_depthStencilView);
}

void Renderer::SetDepthStencilView(ID3D11DepthStencilView * pDepthStencilview)
{
	if (pDepthStencilview == nullptr) {
		m_depthStencilView = m_myDepthStencilView;
	}
	else {
		m_depthStencilView = pDepthStencilview;
	}
}


void Renderer::CreateDepthStencilView()
{
	HRESULT result = 0;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	// Initialize the description of the depth buffer.
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// Set up the description of the depth buffer.
	depthBufferDesc.Width = m_screenW;
	depthBufferDesc.Height = m_screenH;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// Create the texture for the depth buffer using the filled out description.
	result = m_device->CreateTexture2D(&depthBufferDesc, nullptr, &m_depthStencilBuffer);
	if (FAILED(result))
		THROW_EX_HRESULT(InitializeRendererException, result);

	m_depthStencilState = CreateStencilState(true);
	m_depthStencilStateWriteDisabled = CreateStencilState(false);

	// Set the depth stencil state.
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	// Initialize the depth stencil view.
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// Set up the depth stencil view description.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// Create the depth stencil view.
	result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_myDepthStencilView);
	if (FAILED(result))
		THROW_EX_HRESULT(InitializeRendererException, result);
}

// folyt kov
ID3D11DepthStencilState* Renderer::CreateStencilState(bool isEnable)
{
	HRESULT result;
	D3D11_DEPTH_STENCIL_DESC dsDesc;

	ZeroMemory(&dsDesc, sizeof(dsDesc));

	dsDesc.DepthEnable = isEnable;
	dsDesc.DepthWriteMask = isEnable ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
	dsDesc.DepthFunc = isEnable ? D3D11_COMPARISON_LESS : D3D11_COMPARISON_ALWAYS;

	// Stencil test parameters
	dsDesc.StencilEnable = false;
	dsDesc.StencilReadMask = 0xFF;
	dsDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create depth stencil state
	ID3D11DepthStencilState * pDSState;

	result = m_device->CreateDepthStencilState(&dsDesc, &pDSState);

	if (FAILED(result))
		THROW_EX_HRESULT(InitializeRendererException, result);

	return pDSState;
}

// TODO put this into a spearate test framework or so 
// because of stb_image implementation
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

void Renderer::SaveScreenshot(const char * filename)
{
	const int &width = m_viewport.Width;
	const int &height = m_viewport.Height;

	ID3D11Texture2D* pSurface;
	HRESULT hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pSurface));
	if (pSurface)
	{
		const size_t size = width * height;
		uint8_t *captureData = new uint8_t[size * 4];

		ID3D11Texture2D* pNewTexture = NULL;

		D3D11_TEXTURE2D_DESC description;
		pSurface->GetDesc(&description);
		description.BindFlags = 0;
		description.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
		description.Usage = D3D11_USAGE_STAGING;

		HRESULT hr = m_device->CreateTexture2D(&description, NULL, &pNewTexture);
		if (pNewTexture)
		{
			m_deviceContext->CopyResource(pNewTexture, pSurface);
			D3D11_MAPPED_SUBRESOURCE resource;
			unsigned int subresource = D3D11CalcSubresource(0, 0, 0);
			HRESULT hr = m_deviceContext->Map(pNewTexture, subresource, D3D11_MAP_READ_WRITE, 0, &resource);

			const int pitch = width << 2;
			const unsigned char* source = static_cast<const unsigned char*>(resource.pData);
			unsigned char* dest = captureData;
			for (int i = 0; i < height; ++i)
			{
				memcpy(dest, source, width * 4);
				source += resource.RowPitch;
				dest += pitch;
			}

			int res = stbi_write_png(filename, width, height, 4, captureData, pitch);

			delete[] captureData;

			return;
		}
	}
}
