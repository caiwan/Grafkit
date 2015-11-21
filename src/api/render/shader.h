
#pragma once 

#include "../core/reference.h"
#include "dxtypes.h"
#include "../core/exceptions.h"

namespace FWrender {

	class ParameterManager {
		// + parameter manager
	};

	/// @todo: ez a shader csak vagy VS vagy FS shadert tartalmazzon 
	class Shader : virtual public Referencable{
	public:

		/// @todo ezt innen el kell pakoni - egyik felet a node-ba, a masikat a kameraba
		struct MatrixBufferType
		{
			matrix world;
			matrix view;
			matrix projection;
		};

		enum ShaderType_e {
			ST_NONE = 0,
			ST_Vertex,
			ST_Fragment,
			ST_COUNT
		};

	public:
		Shader();
		virtual ~Shader();

		/**
			@param device device context
			@param vsEntry entry point of vertex shader
			@param fsEntry entry point of fragment/pixel shader
			@param vsFile source file of vertex shader
			@param fsFile source file of fragment/pixel shader. NULL if shared with vertex shader
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

// Definition of exceptions

DEFINE_EXCEPTION(MissingShaderExcepotion, 1100, "Missing shader file")

///@todo ezeket at kell pakolni a shader exceptionbe majd
DEFINE_EXCEPTION(VSCrerateException, 1101, "Could not create vertex shader")
DEFINE_EXCEPTION(FSCrerateException, 1102, "Could not create framgent shader")

DEFINE_EXCEPTION(InputLayoutCreateException, 1103, "Could not create input layout")
DEFINE_EXCEPTION(ConstantBufferCreateException, 1104, "Could not create constant buffer")
DEFINE_EXCEPTION(SamplerStateCreateException, 1105, "Could not create sampler state")
DEFINE_EXCEPTION(ConstantBufferLocateException, 1106, "Could not locate constant buffer")

///@Todo ennek teljesen sajat exceptiont kell definialni
DEFINE_EXCEPTION(ShaderException, 1200, "Could not compile shader")
