/**
	... 
*/
#ifndef _Renderer_H_
#define _Renderer_H_

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <DXGI1_2.h>

#include "../stdafx.h"

#include "../utils/reference.h"
#include "../utils/exceptions.h"
#include "../utils/memory.h"


#define RENDER_TARGET_MAX 8

using namespace DirectX;

namespace Grafkit {

	class Camera;
	class Model;
	class Shader;
	class Texture2D;

	// typedefs for reference counting
	typedef Ref<Camera> CameraRef;
	typedef Ref<Model> ModelRef;

	// class ShaderRef;
	// typedef Ref<Texture> TextureRef;
	// class TextureRef;

	/**
		Core renderer
		**Hack**: Extended ID3D11Device via reference counting, gives direct access to the deive with `operator ->` .
	*/
# if 1 // sometimes it comflicts with MFC's new and delete; please mind it. 
	__declspec(align(16)) class Renderer : public Ref<ID3D11Device>, public AlignedNew<Renderer>
#else
	class Renderer : Ref<ID3D11Device>
#endif
	{
	public:
		Renderer();
		virtual ~Renderer();

		int Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, float aspectw = -1, float aspecth = -1);
		void Shutdown();

		void SetViewport(int screenW, int screenH, int offsetX = 0, int offsetY = 0);
		void SetViewportAspect(float aspectW, float aspectH);

		// --- operations 
		void BeginScene(float red, float green, float blue, float alpha);
		void BeginScene();
		void BeginSceneDev();
		void EndScene();

		void ToggleDepthWrite(bool isEanbled);

		// --- setters

		void ApplyRenderTargetView(size_t count = 1);
		void SetRenderTargetView(ID3D11RenderTargetView* pRenderTargetView = nullptr, size_t n = 0);

		// --- getters
		ID3D11Device* GetDevice() { return this->m_device; }
		operator ID3D11Device * const &() { return this->m_device; }

		ID3D11DeviceContext* GetDeviceContext() { return this->m_deviceContext; }
		operator ID3D11DeviceContext * const &() { return this->m_deviceContext; }

		void GetWorldMatrix(matrix&);

		void GetVideoCardInfo(char* dest);

		void GetScreenSize(int &screenW, int &screenH) { screenW = m_screenW; screenH = m_screenH; }
		void GetScreenSizef(float &screenW, float &screenH) { screenW = m_screenW; screenH = m_screenH; }

		void GetViewportSize(int &viewW, int &viewH) { viewW = m_viewport.Width; viewH = m_viewport.Height; }
		void GetViewportSizef(float &viewW, float &viewH) { viewW = (float)m_viewport.Width; viewH = (float)m_viewport.Height; }

		float GetAspectRatio() {return (float)m_viewport.Width / (float)m_viewport.Height;}

		//float GetViewportAspect();

	protected:
		bool m_vsync_enabled;
		char m_videoCardDescription[128];
		IDXGISwapChain* m_swapChain;
		ID3D11Device* m_device;
		ID3D11DeviceContext* m_deviceContext;
		ID3D11RenderTargetView *m_renderTargetViews[RENDER_TARGET_MAX], *m_myRenderTargetView;
		size_t m_renderTargetViewCount;
		ID3D11Texture2D* m_depthStencilBuffer;
		ID3D11DepthStencilState* m_depthStencilState, *m_depthStencilStateWriteDisabled;
		ID3D11DepthStencilView* m_depthStencilView;
		ID3D11RasterizerState* m_rasterState;
		
		matrix m_worldMatrix;

		DXGI_PRESENT_PARAMETERS m_present;
		D3D11_VIEWPORT m_viewport;
		int m_screenW;
		int m_screenH;

	private:
		ID3D11DepthStencilState* CreateStencilState(bool isWriteEnabled);
	};
}

#define EX_ERROR_RENDERER 1000

DEFINE_EXCEPTION(InitializeRendererException, EX_ERROR_RENDERER + 0, "Failed to initialize renderer object");
DEFINE_EXCEPTION(ResizeRenderSurfaceException, EX_ERROR_RENDERER + 1, "Can not resize render surface");

#endif
