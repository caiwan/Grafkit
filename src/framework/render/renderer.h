#pragma once

#include <chrono>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <DXGI1_2.h>

#include "dxtypes.h"

#include "core/reference.h"
#include "core/exceptions.h"
#include "core/memory.h"

#define RENDER_TARGET_MAX D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT

namespace Grafkit {
	/**
		Core renderer
		**Hack**: Extended ID3D11Device via reference counting, gives direct access to the deive with `operator ->` .
	*/
	__declspec(align(16)) 
    class Renderer: public AlignedNew<Renderer>
    {
	public:
		Renderer();

		Renderer(Renderer&) = delete;
		Renderer& operator= (const Renderer&) = delete;

		virtual ~Renderer();

		int Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, float aspectw = -1, float aspecth = -1);
		void Shutdown();

		void Resize(int screenW, int screenH);

		void SetViewport(int screenW, int screenH, int offsetX = 0, int offsetY = 0);
		void SetViewportAspect(float aspectW, float aspectH);

		// --- operations 
		void BeginScene(float red, float green, float blue, float alpha);
		void BeginScene();
		void BeginSceneDev();
		void EndScene();

		void ToggleDepthWrite(bool isEanbled);

		// TODO: put this to a different file or module 
		void SaveScreenshot(const std::string & filename) const;

		// --- setters

		void ApplyRenderTargetView(size_t count = 1);
		void SetDepthStencilView(const ComPtr<ID3D11DepthStencilView> & pDepthStencilView = nullptr);
		void SetRenderTargetView(const ComPtr<ID3D11RenderTargetView> & pRenderTargetView = nullptr, size_t bindId = 0);

		// --- getters
        const ComPtr<ID3D11Device> & GetDevice() const { return this->m_device; }
        const ComPtr<ID3D11DeviceContext> & GetDeviceContext() const { return this->m_deviceContext; }
        
        const ComPtr<ID3D11Device> & operator->() const { return m_device; }
        const ComPtr<ID3D11Device> & operator*() const { return m_device; }

		std::string GetVideoCardInfo() const;

		void GetScreenSize(int &screenW, int &screenH) const { screenW = m_screenW; screenH = m_screenH; }
		void GetScreenSizef(float &screenW, float &screenH) const { screenW = float(m_screenW); screenH = float(m_screenH); }

		void GetViewportSize(int &viewW, int &viewH) const { viewW = int(m_viewport.Width); viewH = int(m_viewport.Height); }
		void GetViewportSize(int &viewW, int &viewH, int &viewX, int &viewY) const { viewW = int(m_viewport.Width), viewH = int(m_viewport.Height), viewX = int(m_viewport.TopLeftX), viewY = int(m_viewport.TopLeftY); }
		void GetViewportSizef(float &viewW, float &viewH) const { viewW = float(m_viewport.Width); viewH = static_cast<float>(m_viewport.Height); }

		float GetAspectRatio() const {return float(m_viewport.Width) / float(m_viewport.Height);}

		float GetDeltaTime() const { return m_lastDeltaTime; }

    private:
		bool m_vsync_enabled;
		char m_videoCardDescription[256];
        ComPtr<ID3D11Device> m_device;
		ComPtr<ID3D11DeviceContext> m_deviceContext;
		ComPtr<IDXGISwapChain> m_swapChain;
        ID3D11RenderTargetView* m_renderTargetViews[RENDER_TARGET_MAX];
        ComPtr<ID3D11RenderTargetView> m_myRenderTargetView;
		size_t m_renderTargetViewCount;
		ComPtr<ID3D11Texture2D> m_depthStencilBuffer;
		ComPtr<ID3D11DepthStencilState> m_depthStencilState, m_depthStencilStateWriteDisabled;
		ComPtr<ID3D11DepthStencilView> m_myDepthStencilView;
		ComPtr<ID3D11DepthStencilView> m_depthStencilView;
		ComPtr<ID3D11RasterizerState> m_rasterState;

		DXGI_PRESENT_PARAMETERS m_present;
		D3D11_VIEWPORT m_viewport;
		int m_screenW;
		int m_screenH;

		float m_lastDeltaTime;
		std::chrono::time_point<std::chrono::system_clock> m_lastTimePoint;

		void CreateDepthStencilView();

		ComPtr<ID3D11DepthStencilState> CreateStencilState(bool isWriteEnabled) const;
	};
}


