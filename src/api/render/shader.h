
#pragma once 

#include <vector>
#include <map>

#include <d3d11shader.h>

#include "dxtypes.h"
#include "renderer.h"

#include "../core/renderassets.h"

#include "reference.h"
#include "exceptions.h"

#include "texture.h"

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

	class Shader;

	//class ShaderLoader : public FWassets::IRenderAssetGenerator
	//{
	//	// implement tis 'ting
	//};

	class Shader : virtual public Referencable, virtual public FWassets::IRenderAsset
	{
		friend class ShaderRef;

		public:
			class ConstantBufferElement;
			class ConstantBufferRecord;
			class BoundResourceRecord;

		public:
			Shader();
			virtual ~Shader();

			/**
				@param device device context
				@param entry entry point of vertex shader
				@param file source file of vertex shader
				@param type type of shader @see FWrender::ShaderType_e
			*/
			void LoadFromFile(ID3D11Device* const & device, LPCSTR entry, LPCWCHAR file, ShaderType_e type);
			void LoadFromMemory(ID3D11Device* const & device, LPCSTR entry, LPCSTR source, size_t size, ShaderType_e type);

			void Shutdown();
			void Render(ID3D11DeviceContext* deviceContext);

			enum ShaderType_e GetShaderType() { return this->m_type; }

			virtual enum RA_type_e GetBucketID() { return FWassets::IRenderAsset::RA_TYPE_Shader; }

		private:
			void CompileShader(ID3D11Device * const & device, ID3D10Blob* shaderBuffer);

		public:

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
			ConstantBufferRecord &operator[] (size_t id);
			size_t GetConstantBufferCount() { return this->m_vBuffers.size(); }

			size_t GetBResourceCount() { return this->m_vBResources.size(); }
			BoundResourceRecord & GetBResource(const char*const name);
			BoundResourceRecord & GetBResource(size_t id);

			size_t GetILayoutElemCount() { return this->m_mapInputElems.size(); }
			InputElementRecord getILayoutElem(size_t index) { return this->m_mapInputElems[index]; }

			// set input layout
			void setInputLayout(ID3D11InputLayout* pLayout) { this->m_layout = pLayout; }
		
			// set shader resource

			// set out sampler 

		private:
			void DispatchShaderErrorMessage(ID3D10Blob* errorMessage, LPCWCHAR file, LPCSTR entry);
			void GetDXGIFormat(D3D11_SIGNATURE_PARAMETER_DESC pd, DXGI_FORMAT &res, UINT &byteWidth);
			void BuildReflection(ID3D11Device* device, ID3D10Blob* shaderBuffer);

		private:
			ShaderType_e m_type;
			ID3D11VertexShader* m_vxShader;
			ID3D11PixelShader* m_pxShader;

			ID3D11InputLayout* m_layout;

			ID3D11ShaderReflection *m_pReflector;

			typedef std::vector<InputElementRecord> inputElements_t;
			inputElements_t m_mapInputElems;

			typedef std::map<std::string, size_t> bufferMap_t;
			bufferMap_t m_mapBuffers;
			std::vector<ConstantBufferRecord> m_vBuffers;

			std::vector<std::string> m_inputNames;

			typedef std::map<std::string, size_t> bResourceMap_t;
			bResourceMap_t m_mapBResources;
			std::vector<BoundResourceRecord> m_vBResources;

		public:
			// ==========================================================================================
			/**
			A class that contains and manages one entire constant buffer with its elements from
			the reflection record for a certain constant buffer and aids to set variables into it
			*/

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

					size_t GetElementCount() { return this->m_vConstVars.size(); }
					ConstantBufferElement& operator[](const char* name);
					ConstantBufferElement& operator[](size_t id);

					D3D11_SHADER_BUFFER_DESC& const GetBufferDesc() { return this->m_description; }

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
					typedef std::map<std::string, size_t> cb_variableMap_t;
					cb_variableMap_t m_mapConstVars;

					// a getterek miatt kell
					std::vector<ConstantBufferElement> m_vConstVars;

			};

			/**
			Helps to bind one element from the constant buffer
			*/
			class ConstantBufferElement
			{
				friend class ConstantBufferRecord;
				public:
					ConstantBufferElement();

				protected:
					ConstantBufferElement(Shader::ConstantBufferRecord* parent_record, ID3D11ShaderReflectionVariable* shader_variable);

				public:
					/// @todo implement 
					void operator= (float v);

					void operator= (float3 v);
					void operator= (float2 v);
					void operator= (float4 v);

					/// @todo 16-os alignmentet meg kell szerelni
					//void operator= (FWmath::Matrix v);

					void set(float v1);
					void set(float v1, float v2);
					void set(float v1, float v2, float v3);
					void set(float v1, float v2, float v3, float v4);

					D3D11_SHADER_VARIABLE_DESC & const GetVarDesc();
					D3D11_SHADER_TYPE_DESC & const GetTypeDesc();

				protected:
					Shader::ConstantBufferRecord* m_pBufferRecord;

					D3D11_SHADER_VARIABLE_DESC m_var_desc;
					D3D11_SHADER_TYPE_DESC m_type_desc;
			};

			/**
			Azt. 
			*/
			class BoundResourceRecord {
				friend class Shader;
				public:
					BoundResourceRecord();
					BoundResourceRecord(D3D11_SHADER_INPUT_BIND_DESC desc);

					D3D11_SHADER_INPUT_BIND_DESC & GetDesc() { return m_desc; }
					void SetTexture(TextureRef texture = nullptr) { m_rBoundTexture = texture; }

					int IsValid() { return m_is_valid; }

				protected:
					TextureRef m_rBoundTexture;
					D3D11_SHADER_INPUT_BIND_DESC m_desc;

					int m_is_valid;
			};

		};

		/// enhance Reference with operator [] to acces the shader's indides, avoiding dereferencing
		class ShaderRef : public Ref<Shader> {
			friend class ShaderRef;

		public:
			Shader::ConstantBufferRecord& operator[](const char *name){
				return this->ptr->operator[](name);
			}

			Shader::ConstantBufferRecord& operator[](size_t id) {
				return this->ptr->operator[](id);
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

		typedef struct {
			ShaderRef vs, fs;
		} shader_pair_t;

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
DEFINE_EXCEPTION(BoundResourceLocateException, 1109, "Could not locate bound resouce")

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

