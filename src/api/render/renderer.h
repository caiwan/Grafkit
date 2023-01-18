/**
	... 
*/
#ifndef _Renderer_H_
#define _Renderer_H_

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include "dxtypes.h"
#include "reference.h"

#include "memory_align.h"

using namespace DirectX;

namespace FWrender {

	class Camera;
	class Model;
	class Shader;
	class Texture;

	// typedefs for reference counting
	typedef Ref<Camera> CameraRef;
	typedef Ref<Model> ModelRef;
	// class ShaderRef;
	typedef Ref<Texture> TextureRef;

	/**
		Core renderer
		**Hack**: Extended ID3D11Device via reference counting, gives direct access to the deive with `operator ->` .
	*/
# if 1 // sometimes it comflicts with MFC's new and delete; please mind it. 
	__declspec(align(16)) class Renderer : Ref<ID3D11Device>, public AlignedNew<Renderer>
#else
	class Renderer : Ref<ID3D11Device>
#endif
	{
	public:
		Renderer();
		virtual ~Renderer();

		///@todo  alignmenttel kezdeni kell valamit majd 
		// void* operator new(size_t);
		// void operator delete(void*);

		///@todo viewporttal kell kezdnei valmait majd 
		int Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen);
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

		void GetWorldMatrix(matrix&);

		void GetVideoCardInfo(char*);

		void getScreenSize(int &screenW, int &screenH);
		void getScreenSize_f(float &screenW, float &screenH) {
			int w=0, h=0;
			this->getScreenSize(w, h);
			screenW = (float)w, screenH = (float)h;
		}

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
		
		matrix m_worldMatrix;
	};
}
#endif