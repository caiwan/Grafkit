
#pragma once 

#include <vector>
#include <map>

// #pragma lib(dxguid.lib.)
#include <d3d11shader.h>

#include "dxtypes.h"
#include "../render/renderer.h"

#include "reference.h"
#include "exceptions.h"

#include "../utils/stringutils.h"

namespace FWrender {

	class Shader;
	class ShaderRef;

	enum ShaderType_e {
		ST_NONE = 0,
		ST_Vertex,
		ST_Pixel,
		ST_Compute,
		ST_COUNT
	};

	class Shader : virtual public Referencable {
		friend class ShaderRef;

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
		void LoadFromMemory(ID3D11Device* device, LPCSTR entry, LPCSTR source, size_t size, ShaderType_e type);

		void Shutdown();
		void Render(ID3D11DeviceContext* deviceContext);

		enum ShaderType_e getShaderType() { return this->m_type; }

	private:
		void CompileShader(ID3D11Device* device, ID3D10Blob* shaderBuffer);

	public:

		// ==========================================================================================
		/**
		A class that contains and manages one entire constant buffer with its elements from
		the reflection record for a certain constant buffer and aids to set variables into it
		*/
		class ConstantBufferElement;

		class ConstantBufferRecord
		{
			friend class Shader;

		public:
			// This class has to have a public accessable default constructor due to std::map
			ConstantBufferRecord();
		protected:
			ConstantBufferRecord(ID3D11Device* device, ID3D11ShaderReflectionConstantBuffer * pConstantBuffer);

		public:
			void Set(void* pData);
			void Set(void* pData, size_t offset, size_t width);

		private:
			void Map();
			void Unmap();
			void *GetMappedPtr();

			ID3D11DeviceContext* m_pDC;
			D3D11_MAPPED_SUBRESOURCE m_mappedResource;
			D3D11_SHADER_BUFFER_DESC m_description;
			ID3D11Buffer *m_buffer;
			UINT m_slot;

		protected:
			typedef std::map<std::string, ConstantBufferElement> cb_variableMap_t;
			cb_variableMap_t m_mapConstantVariables;

		};

		/**
		Helps to bind one element from the constant buffer
		*/
		class ConstantBufferElement 
		{
			friend class ConstantBufferRecord;
		public:
			ConstantBufferElement(Shader::ConstantBufferRecord* parent_record = nullptr);

		protected:
			ConstantBufferElement(Shader::ConstantBufferRecord* parent_record, ID3D11ShaderReflectionVariable* shader_variable);

			// void operator= (float v);

			//void operator= (float2 v);
			//void operator= (float3 v);
			//void operator= (float4 v);
			// 16-os alignmentet meg kell szerelni
			// void operator= (FWmath::Matrix v);

			// void set(float v1, float v2);
			// void set(float v1, float v2, float v3);
			// void set(float v1, float v2, float v3, float v4);

		protected:
			Shader::ConstantBufferRecord* m_pBufferRecord;
			size_t m_offset;
			size_t m_width;

			D3D11_SHADER_VARIABLE_DESC m_var_desc;
			D3D11_SHADER_TYPE_DESC m_type_desc;
		};
	

		// ==========================================================================================

		/**
		A struct that extends the input layout descriptor
		*/
		struct InputElementRecord{
			D3D11_INPUT_ELEMENT_DESC desc;
			UINT width;
			UINT offset;

			InputElementRecord() {}
		};

		ConstantBufferRecord &operator[] (const char* name);

		size_t getILayoutElemCount() { return this->m_mapInputElems.size(); }
		InputElementRecord getILayoutElem(size_t index) { return this->m_mapInputElems[index]; }

		// set input layout
		void setInputLayout(ID3D11InputLayout* pLayout) { this->m_layout = pLayout; }
		
		// set shader resource

		// set out sampler 

	protected:
		void DispatchShaderErrorMessage(ID3D10Blob* errorMessage, LPCWCHAR file, LPCSTR entry);
		void getDXGIFormat(D3D11_SIGNATURE_PARAMETER_DESC pd, DXGI_FORMAT &res, UINT &byteWidth);
		void BuildReflection(ID3D11Device* device, ID3D10Blob* shaderBuffer);

	private:
		ShaderType_e m_type;
		ID3D11VertexShader* m_vShader;
		ID3D11PixelShader* m_pShader;

		ID3D11InputLayout* m_layout;

		ID3D11ShaderReflection *m_pReflector;

		//typedef std::map<const char*, InputElementRecord, cmp_str> inputElementMap_t;
		typedef std::vector<InputElementRecord> inputElementMap_t;
		inputElementMap_t m_mapInputElems;

		//typedef std::map<const char*, ConstantBufferRecord, cmp_str> bufferMap_t;
		typedef std::map<std::string, ConstantBufferRecord> bufferMap_t;
		bufferMap_t m_mapBuffers;

		std::vector<std::string> m_inputNames;
	};

	/// enhance Reference with operator [] to acces the shader's indides, avoiding dereferencing
	class ShaderRef : public Ref<Shader> {
		friend class ShaderRef;

	public:
		Shader::ConstantBufferRecord& operator[](const char *name){
			return this->ptr->operator[](name);
		}


		// --- compatibility with Ref<T>
		ShaderRef & operator=(Shader* pointer)
		{
			AssingnRef(pointer);
			return *this;
		}

		ShaderRef& operator=(const ShaderRef& other)
		{
			if (&other != this && this->ptr != other.ptr) {
				if (ptr != NULL &&
					ptr->Release() == 0) {
					delete ptr;
				}

				ptr = other.ptr;

				if (ptr != NULL) {
					ptr->AddRef();
				}
			}

			return *this;
		}

		template<typename T1> ShaderRef& operator=(const Ref<T1>& other)
		{
			if (&other != this && this->ptr != other.Get()) {
				if (ptr != NULL &&
					ptr->Release()) {
					delete ptr;
				}

				ptr = other.Get();

				if (ptr != NULL) {
					ptr->AddRef();
				}
			}

			return *this;
		}
	};

	//typedef Ref<Shader> ShaderRef;

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
DEFINE_EXCEPTION(ConstantBufferMapException, 1108, "Could not map constant buffer")

///@Todo ennek teljesen sajat exceptiont kell definialni

#if 1 // NO_ShaderException

DEFINE_EXCEPTION(ShaderException, 1200, "Could not compile shader")

#else 0 // NO_ShaderException

#define HAS_ShaderException

namespace FWdebugExceptions {
	class ShaderException {
		// ... 

	};
}

#endif // NO_ShaderException

