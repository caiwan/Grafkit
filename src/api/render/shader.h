
#pragma once 

#include "dxtypes.h"

namespace FWrender {
	class Shader {
	public:
		struct MatrixBufferType
		{
			matrix world;
			matrix view;
			matrix projection;
		};
	public:
		Shader();
		virtual ~Shader();

		/**
			@param vsEntry entry point of vertex shader
			@param fsEntry entry point of fragment/pixel shader
			@param vsFile source file of vertex shader
			@param fsFile source file of fragment/pixel shader. NULL if shared with vertex shader
			@throws lots of exceptions
		*/
		void LoadFromFile(ID3D11Device* device, LPCSTR vsEntry, LPCSTR fsEntry, LPCWCHAR vsFile, LPCWCHAR fsFile = NULL);
		void Shutdown();
		void Render(ID3D11DeviceContext* deviceContext);

		void SetCameraMatrices(ID3D11DeviceContext* deviceContext, FWmath::Matrix& projection, FWmath::Matrix& world, FWmath::Matrix& view);
		void SetTexture(ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* texture);

	protected:
		void DispatchShaderErrorMessage(ID3D10Blob* errorMessage, LPCWCHAR file, LPCSTR entry);

	private:
		ID3D11VertexShader* m_vertexShader;
		ID3D11PixelShader* m_pixelShader;
		ID3D11InputLayout* m_layout;
		ID3D11Buffer* m_matrixBuffer;

		ID3D11SamplerState* m_sampleState;
	};
}