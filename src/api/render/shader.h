#pragma once 

#include <vector>
#include <map>

#include <d3d11shader.h>

#include "../utils/exceptions.h"
#include "../utils/reference.h"
#include "../utils/resource.h"

#include "dxtypes.h"
#include "renderer.h"


namespace Grafkit {

	class Shader;
	
	enum ShaderType_e {
		ST_NONE = 0,
		ST_Vertex,
		ST_Pixel,
		ST_Geometry,
		ST_Compute,
		
		ST_COUNT
	};

	///opt out operator overlaoding (= and [])
#define SHADER_NO_OPERATOR_ENHANCEMENT

	// ================================================================================================================================
	__declspec(align(16)) class Shader : virtual public Referencable, public AlignedNew<Shader>
	{

	public:
		class ShaderParamManager;
		class ShaderResourceManager;

	public:
		Shader();
		virtual ~Shader();

		/**
			@param device device context
			@param entry entry point of vertex shader
			@param file source file of vertex shader
			@param type type of shader @see Grafkit::ShaderType_e
		*/
		void LoadFromFile(Renderer & device, LPCSTR entry, LPCWCHAR file, ShaderType_e type);
		void LoadFromMemory(Renderer & device, LPCSTR entry, LPCSTR source, size_t size, ShaderType_e type);

		void Shutdown();
		void Render(ID3D11DeviceContext* deviceContext);

		enum ShaderType_e GetShaderType() { return this->m_type; }

		//virtual enum RA_type_e GetBucketID() { return Grafkit::IResource::RA_TYPE_Shader; }

	private:
		void CompileShader(Renderer & device, ID3D10Blob* shaderBuffer);

	public:

		// ==========================================================================================

		/**
		A struct that extends the input layout descriptor
		*/
		struct InputElementRecord{
			D3D11_INPUT_ELEMENT_DESC desc;
			UINT width;
			UINT offset;
		};

		struct OutputTargetRecord {
			D3D11_SIGNATURE_PARAMETER_DESC desc;
		};

		// ----
		// access constant buffers and variables 
		///@todo bounds check
		size_t GetParamCount() { return this->m_cBuffers.size(); }
		size_t GetParamCount(size_t id) { return id>= GetParamCount() ? 0:this->m_cBuffers[id].m_cbVars.size(); }

#ifndef SHADER_NO_OPERATOR_ENHANCEMENT
		ShaderParamManager operator[](const char* name) { return GetParam(std::string(name)); }
		ShaderParamManager operator[](size_t id) { return GetParam(id); }
#endif //SHADER_NO_OPERATOR_ENHANCEMENT

		ShaderParamManager GetParam(const char* name) { return GetParam(std::string(name)); }
		ShaderParamManager GetParam(std::string name);
		ShaderParamManager GetParam(size_t id);
							
		ShaderParamManager GetParam(const char* name, const char* varname) { return GetParam(std::string(name), std::string(name)); }
		ShaderParamManager GetParam(std::string name, std::string varname);
		ShaderParamManager GetParam(size_t id, const char* varname) { return GetParam(id, std::string(varname)); }
		ShaderParamManager GetParam(size_t id, std::string varname);
		ShaderParamManager GetParam(size_t id, size_t vid);

		void SetParamPtr(size_t id, const void * const pData, size_t size = 0, size_t offset = 0);
		void SetParamPtr(size_t id, size_t vid, const void * const pData, size_t size = 0, size_t offset = 0);

	protected:
		void* MapParamBuffer(size_t id, int isDiscard = 1);
		void* GetMappedPtr(size_t id);
		void UnMapParamBuffer(size_t id);
		
	public:

		D3D11_SHADER_BUFFER_DESC GetCBDescription(size_t id) {
			return m_cBuffers[id].m_description;
		}
		
		D3D11_SHADER_VARIABLE_DESC GetCBVariableDescriptor(size_t id, size_t vid) {
			return m_cBuffers[id].m_cbVars[vid].m_var_desc;
		}
		D3D11_SHADER_TYPE_DESC GetCBTypeDescriptor(size_t id, size_t vid) {
			return m_cBuffers[id].m_cbVars[vid].m_type_desc;
		}

		// ----
		// access bounded resources
		///@todo bounds check
		size_t GetBResCount() { return this->m_bResources.size(); }
		ShaderResourceManager GetBRes(const char * name) { return GetBRes(std::string(name)); }
		ShaderResourceManager GetBRes(std::string name);
		ShaderResourceManager GetBRes(size_t id);
		D3D11_SHADER_INPUT_BIND_DESC GetBResDesc(size_t id);

		void SetBResPointer(size_t id, void* ptr);

		// ----
		// access input layout 
		size_t GetILayoutElemCount() { return this->m_mapInputElems.size(); }
		InputElementRecord getILayoutElem(size_t index) { return this->m_mapInputElems[index]; }

		// set input layout
		void setInputLayout(ID3D11InputLayout* pLayout) { this->m_layout = pLayout; }
		
		// set shader resource

		// set out sampler 

	private:
		void DispatchShaderErrorMessage(ID3D10Blob* errorMessage, LPCWCHAR file, LPCSTR entry);
		void GetDXGIFormat(D3D11_SIGNATURE_PARAMETER_DESC pd, DXGI_FORMAT &res, UINT &byteWidth);
		void BuildReflection(Renderer & device, ID3D10Blob* shaderBuffer);

	private:
		ShaderType_e m_type;
		ID3D11VertexShader* m_vxShader;
		ID3D11PixelShader* m_pxShader;

		ID3D11DeviceContext* m_pDC;

	protected:
		ID3D11ShaderReflection *m_pReflector;

		// -- constant buffer
		// Constant buffer variable 

		///@todo a kovetkezo iteracioban nem kell reflektalni teljesen a shadert, eleg csak a buffereket letrehozni CPU oldalon
		
		struct CBVar {
			D3D11_SHADER_VARIABLE_DESC m_var_desc;
			D3D11_SHADER_TYPE_DESC m_type_desc;
		};

		typedef std::map<std::string, size_t> cb_variableMap_t;
		typedef cb_variableMap_t::iterator cb_variableMap_it;

		// constant buffer record 
		struct CBRecord {
			D3D11_MAPPED_SUBRESOURCE m_mappedResource;
			D3D11_SHADER_BUFFER_DESC m_description;
			ID3D11Buffer *m_buffer;
			BYTE *m_cpuBuffer;
			UINT m_slot;

			cb_variableMap_t m_cbVarMap;
			std::vector<CBVar> m_cbVars;
		};

		typedef std::map<std::string, size_t> CBMap_t;
		typedef CBMap_t::iterator CBMap_it_t;

		CBMap_t m_mapCBuffers;
		std::vector<CBRecord> m_cBuffers;

		// -- input layout 
		typedef std::vector<InputElementRecord> inputElements_t;
		inputElements_t m_mapInputElems;
		ID3D11InputLayout* m_layout;

		// -- bounded resource

		struct BResRecord {
			D3D11_SHADER_INPUT_BIND_DESC m_desc;
			void *m_boundSource;
		};

		typedef std::map<std::string, size_t> BResMap_t;
		typedef BResMap_t::iterator BResMap_it_t;
		BResMap_t m_mapBResources;
		
		std::vector<BResRecord> m_bResources;

		// -- output sampler
		std::vector<OutputTargetRecord> m_outputTargets;

		// ================================================================================================================================

		public:

			class ShaderParamManager {
			public:
				ShaderParamManager(Shader* shader = nullptr, size_t id = 0, size_t vid = 0, int is_subtype = 0)
					: m_pShader(shader), m_id(id), m_vid(vid), m_is_subtype(is_subtype)
				{}

				~ShaderParamManager() {}

#ifndef SHADER_NO_OPERATOR_ENHANCEMENT
				inline ShaderParamManager operator[](const char* name) { return Get(name); }
				inline ShaderParamManager operator[](size_t id) { return Get(id); }
#endif //SHADER_NO_OPERATOR_ENHANCEMENT
				
				inline ShaderParamManager Get(const char* name) { return (this->IsValid() && !this->IsSubtype()) ? this->m_pShader->GetParam(m_id, name) : ShaderParamManager(); }
				inline ShaderParamManager Get(size_t id) { return (this->IsValid() && !this->IsSubtype()) ? this->m_pShader->GetParam(m_id, id) : ShaderParamManager(); }

				inline size_t GetVarCount() { return (this->IsValid() && !this->IsSubtype()) ? this->m_pShader->GetParamCount() : 0; }

#ifndef SHADER_NO_OPERATOR_ENHANCEMENT
				void operator= (float v) { SetP(&v); }
			
				void operator= (void* v) { SetP(v); }
				
				void operator= (const float3 &v) { SetP(&v); }
				void operator= (const float2 &v) { SetP(&v); }
				void operator= (const float4 &v) { SetP(&v); }
				void operator= (const matrix &v) { SetP(&v); }
#endif //SHADER_NO_OPERATOR_ENHANCEMENT

				inline void SetP(const void * const pData, size_t width = 0, size_t offset = 0) { if (this->IsValid()) this->IsSubtype() ? this->m_pShader->SetParamPtr(m_id, m_vid, pData, width, offset) : this->m_pShader->SetParamPtr(m_id, pData, width, offset); }

				void SetF(const float v0);
				void SetF(const float v0, const float v1);
				void SetF(const float v0, const float v1, const float v2);
				void SetF(const float v0, const float v1, const float v2, const float v3);

				
				inline D3D11_SHADER_BUFFER_DESC GetBufferDesc() { return (this->IsValid()) ? this->m_pShader->GetCBDescription(m_id) : D3D11_SHADER_BUFFER_DESC(); }
				
				inline D3D11_SHADER_VARIABLE_DESC GetVarDesc() { return (this->IsValid()) ? this->m_pShader->GetCBVariableDescriptor(m_id, m_vid) : D3D11_SHADER_VARIABLE_DESC(); }
				inline D3D11_SHADER_TYPE_DESC  GetTypeDesc() { return (this->IsValid()) ? this->m_pShader->GetCBTypeDescriptor(m_id, m_vid) : D3D11_SHADER_TYPE_DESC(); }
				

				operator D3D11_SHADER_BUFFER_DESC () { this->GetBufferDesc(); }
				
				operator D3D11_SHADER_VARIABLE_DESC  () { this->GetVarDesc(); }
				operator D3D11_SHADER_TYPE_DESC   () { this->GetTypeDesc(); }
				

				inline int IsValid() { return (m_pShader != nullptr); }
				inline int IsSubtype() { return m_is_subtype; }

			private:
				Shader * m_pShader;

				int m_is_subtype;
				size_t m_id, m_vid;

			};

			class ShaderResourceManager {
			public:
				ShaderResourceManager(Shader* shader = nullptr, size_t id = 0) : 
					m_pShader(shader), m_id(id)
				{}

				~ShaderResourceManager() {}

				
#ifndef SHADER_NO_OPERATOR_ENHANCEMENT
				//void operator= (void* v) { Set(v); }
				void operator= (ID3D11ShaderResourceView* v) { Set(v); }
				void operator= (ID3D11SamplerState* v) { Set(v); }
#endif //SHADER_NO_OPERATOR_ENHANCEMENT

				//void Set(void * p) {this->m_pShader->SetBResPointer(m_id, p); }
				void Set(ID3D11ShaderResourceView* p) { if (IsValid()) this->m_pShader->SetBResPointer(m_id, p); }
				void Set(ID3D11SamplerState* p) { if (IsValid()) this->m_pShader->SetBResPointer(m_id, p); }

				inline D3D11_SHADER_INPUT_BIND_DESC GetDescriptor() { if (IsValid()) m_pShader->GetBResDesc(m_id); }

				int IsValid() { return (m_pShader != nullptr); }

			private:

				void Set(void* p) { if (IsValid()) this->m_pShader->SetBResPointer(m_id, p); }

				Shader * m_pShader;
				size_t m_id;
			};
	};


	// ================================================================================================================================

	typedef Ref<Shader> ShaderRef;
	typedef Grafkit::Resource<Shader> ShaderRes;
	typedef Ref<ShaderRes> ShaderResRef;

}

// Definition of exceptions

#define EX_ERROR_SHADER 1100

DEFINE_EXCEPTION(MissingShaderException, EX_ERROR_SHADER + 0, "Missing shader file")

///@todo ezeket at kell pakolni a shader exceptionbe majd
DEFINE_EXCEPTION(VSCrerateException, EX_ERROR_SHADER + 1, "Could not create vertex shader")
DEFINE_EXCEPTION(FSCrerateException, EX_ERROR_SHADER + 2, "Could not create framgent shader")

DEFINE_EXCEPTION(InputLayoutCreateException, EX_ERROR_SHADER + 3, "Could not create input layout")
DEFINE_EXCEPTION(ConstantBufferCreateException, EX_ERROR_SHADER + 4, "Could not create constant buffer")
DEFINE_EXCEPTION(ConstantBufferLocateException, EX_ERROR_SHADER + 5, "Could not locate constant buffer")
DEFINE_EXCEPTION(ConstantBufferMapException, EX_ERROR_SHADER + 6, "Could not map constant buffer")
DEFINE_EXCEPTION(BoundResourceLocateException, EX_ERROR_SHADER + 7, "Could not locate bound resouce")

///@Todo ennek teljesen sajat exceptiont kell definialni

#if 1 // NO_ShaderException

DEFINE_EXCEPTION(ShaderException, 1200, "Could not compile shader")

#else 0 // NO_ShaderException

#define _HAS_ShaderException

namespace FWdebugExceptions {
	class ShaderException /*:*/ {
		// ... 

	};
}

#endif // NO_ShaderException

