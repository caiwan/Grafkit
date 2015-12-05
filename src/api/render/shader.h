
#pragma once 

#include <vector>
#include <map>

#include "../core/reference.h"
#include "dxtypes.h"
#include "../render/renderer.h"
#include "../core/exceptions.h"

// #pragma lib(dxguid.lib.)
#include <d3d11shader.h>


// DHM - Dirty Hack of Mine
// http://stackoverflow.com/questions/4157687/using-char-as-a-key-in-stdmap

/// @todo move the fuck away
struct cmp_str
{
	bool operator()(char const *a, char const *b)
	{
		return std::strcmp(a, b) < 0;
	}
};


namespace FWrender {

	class Shader;

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

		enum ShaderType_e getShaderType() { return this->m_type; }

	public:
		
		/**
		A class that holds the reflection record for a certain constant buffer and aids to set variables into it
		*/
		class ConstantBufferRecord {
			friend class Shader;
		
		public:
			// This class has to have a public accessable default constructor due to std::map
			ConstantBufferRecord() : ConstantBufferRecord(NULL, NULL) {/*fucings bele*/ }
		protected:
			ConstantBufferRecord(ID3D11Device* device, ID3D11ShaderReflectionConstantBuffer * pConstantBuffer);

			// void setDC(ID3D11DeviceContext* pd3dic) { this->m_pDC = pd3dic; }
		
		public:
			// void operator= (float v);

			//void operator= (float2 v);
			//void operator= (float3 v);
			//void operator= (float4 v);
			// 16-os alignmentet meg kell szerelni
			// void operator= (FWmath::Matrix v);

			// void set(float v1, float v2);
			// void set(float v1, float v2, float v3);
			// void set(float v1, float v2, float v3, float v4);

			void set(void*); 

		private:
			ID3D11DeviceContext* m_pDC;
			// Shader *m_parent;
			D3D11_SHADER_BUFFER_DESC m_description;
			ID3D11Buffer *m_buffer;

		private:
			ConstantBufferRecord(ConstantBufferRecord&) {}
			ConstantBufferRecord& operator= (ConstantBufferRecord&) {}
		};

		ConstantBufferRecord& operator[] (const char* name);

		// set input layout
		// set out sampler 

	protected:
		void DispatchShaderErrorMessage(ID3D10Blob* errorMessage, LPCWCHAR file, LPCSTR entry);
		void BuildReflection(ID3D11Device* device);

	private:
		ShaderType_e m_type;
		ID3D11VertexShader* m_vShader;
		ID3D11PixelShader* m_pShader;
		ID3D11ShaderReflection *m_pReflector;

		typedef std::map<const char*, ConstantBufferRecord, cmp_str> bufferMap_t;
		bufferMap_t m_mapBuffers;


		// ID3D11DeviceContext* m_pDeviceContext;
	};

	/*
	class ShaderRef : public Ref<Shader> {
		/// enhance Reference with operator [] to acces the shader's indides, without dereferencing
		// ... 
		///@todo a = operator nem toltodik at -> tagfuggveny elrejtes? 
	};
	*/

	typedef Ref<Shader> ShaderRef;

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

