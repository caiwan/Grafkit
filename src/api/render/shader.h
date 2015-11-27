
#pragma once 

#include <vector>

#include "../core/reference.h"
#include "dxtypes.h"
#include "../render/renderer.h"
#include "../core/exceptions.h"


namespace FWrender {

	class Shader;

	//class ParameterManager {
	//	friend class Shader;
	//public:
	//	// --- d3d reflection goez here --- 
	//	// + parameter manager
	//};

	enum ShaderType_e {
		ST_NONE = 0,
		ST_Vertex,
		ST_Pixel,
		ST_COUNT
	};

	class Shader : virtual public Referencable {
	public:

		/// @todo ezt innen el kell pakoni - egyik felet a node-ba, a masikat a kameraba
		/*struct MatrixBufferType
		{
			matrix world;
			matrix view;
			matrix projection;
		};*/

	public:
		Shader();
		virtual ~Shader();

		/**
			@param device device context
			@param entry entry point of vertex shader
			@param file source file of vertex shader
			@param type type of shader @see FWrender::ShaderType_e
		*/
		void LoadFromFile(ID3D11Device* device, LPCSTR entry, LPCWCHAR file, ShaderType_e type);

		void Shutdown();
		void Render(ID3D11DeviceContext* deviceContext);

	public:
		class ShaderVariable {
			friend class Shader;
		private:
			ShaderVariable(D3D11_SHADER_BUFFER_DESC descriptor);

			D3D11_SHADER_BUFFER_DESC descriptor;
		public:
			void operator= (float v);
			void operator= (float2 v);
			void operator= (float3 v);
			void operator= (float4 v);
			void operator= (FWmath::Matrix v);

			void set(float v1, float v2);
			void set(float v1, float v2, float v3);
			void set(float v1, float v2, float v3, float v4);

			void set(void*); 
		};

		ShaderVariable operator[] (const char*); 

		// set input layout
		// set out sampler 

	protected:
		void DispatchShaderErrorMessage(ID3D10Blob* errorMessage, LPCWCHAR file, LPCSTR entry);
		void BuildReflection();

		struct ConstantBufferLayout {
			D3D11_SHADER_BUFFER_DESC Description;
			std::vector<D3D11_SHADER_VARIABLE_DESC> Variables;
			std::vector<D3D11_SHADER_TYPE_DESC> Types;
		};

	private:
		ID3D11VertexShader* m_vShader;
		ID3D11PixelShader* m_pShader;
		ID3D11ShaderReflection *m_pReflector;

		std::vector<struct ConstantBufferLayout>  m_constantBuffers;
	};

	struct shader_pair {
		ShaderRef vs, fs;
	};

}

// Definition of exceptions

DEFINE_EXCEPTION(MissingShaderExcepotion, 1100, "Missing shader file")

///@todo ezeket at kell pakolni a shader exceptionbe majd
DEFINE_EXCEPTION(VSCrerateException, 1101, "Could not create vertex shader")
DEFINE_EXCEPTION(FSCrerateException, 1102, "Could not create framgent shader")

DEFINE_EXCEPTION(InputLayoutCreateException, 1103, "Could not create input layout")
DEFINE_EXCEPTION(ConstantBufferCreateException, 1104, "Could not create constant buffer")
DEFINE_EXCEPTION(ConstantBufferLocateException, 1106, "Could not locate constant buffer")

///@Todo ennek teljesen sajat exceptiont kell definialni
DEFINE_EXCEPTION(ShaderException, 1200, "Could not compile shader")

