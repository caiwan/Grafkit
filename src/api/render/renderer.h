/**
	... 
*/
#ifndef _Renderer_H_
#define _Renderer_H_

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include "dxtypes.h"
#include "../core/reference.h"

using namespace DirectX;

namespace FWrender {

	class Camera;
	class Model;
	class Shader;
	class Texture;

	// typedefs for reference counting
	typedef Ref<Camera> CameraRef;
	typedef Ref<Model> ModelRef;
	typedef Ref<Shader> ShaderRef;
	typedef Ref<Texture> TextureRef;

	/**
		Core renderer
		**Hack**: Extended ID3D11Device via reference counting, gives direct access to the deive with `operator ->` .
	*/
	class Renderer : Ref<ID3D11Device>
	{
	public:
		Renderer();
		~Renderer();

		void* operator new(size_t);
		void operator delete(void*);

		int Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, float screenDepth, float screenNear);
		void Shutdown();

		// --- operations 
		void BeginScene(float red, float green, float blue, float alpha);
		void BeginScene();
		void EndScene();

		// --- getters
		ID3D11Device* GetDevice() { return this->m_device; }
		operator ID3D11Device *() { return this->m_device; }

		ID3D11DeviceContext* GetDeviceContext() { return this->m_deviceContext; }
		operator ID3D11DeviceContext*() { return this->m_deviceContext; }

		void GetProjectionMatrix(XMMATRIX&);
		void GetWorldMatrix(XMMATRIX&);
		void GetOrthoMatrix(XMMATRIX&);

		void GetVideoCardInfo(char*);

	private:
		bool m_vsync_enabled;
		char m_videoCardDescription[128];
		IDXGISwapChain* m_swapChain;
		ID3D11Device* m_device;
		ID3D11DeviceContext* m_deviceContext;
		ID3D11RenderTargetView* m_renderTargetView;
		ID3D11Texture2D* m_depthStencilBuffer;
		ID3D11DepthStencilState* m_depthStencilState;
		ID3D11DepthStencilView* m_depthStencilView;
		ID3D11RasterizerState* m_rasterState;
		XMMATRIX m_projectionMatrix;
		XMMATRIX m_worldMatrix;
		XMMATRIX m_orthoMatrix;
	};
}
#endif