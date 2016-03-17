#pragma once 

#include <vector>
#include <map>

#include <d3d11shader.h>

#include "reference.h"
#include "exceptions.h"

#include "dxtypes.h"
#include "renderer.h"
#include "texture.h"

#include "../core/resource.h"
#include "../core/ResourceBuilder.h"

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

	class Shader;
	class ShaderRes;
	class ShaderResRef;

	class ShaderParamManager {
		// ... 
	public:
		ShaderParamManager(Renderer &render, Shader*shader = nullptr, size_t id = 0, size_t vid = 0, int is_subtype =0);

		void operator= (float v) { Set(&v); }

		void operator= (const float3 &v) { Set(&v); }
		void operator= (const float2 &v) { Set(&v); }
		void operator= (const float4 &v) { Set(&v); }
		void operator= (const matrix &v) { Set(&v); }

		/// @todo 16-os alignmentet meg kell szerelni
		//void operator= (Grafkit::Matrix v);

		void Set(const void * const pData);
		void Set(const void * const pData, size_t offset, size_t width);

		void Set(const float v0);
		void Set(const float v0, const float v1);
		void Set(const float v0, const float v1, const float v2);
		void Set(const float v0, const float v1, const float v2, const float v3);

		D3D11_MAPPED_SUBRESOURCE GetappedResourceDesc();
		D3D11_SHADER_BUFFER_DESC GetBufferDesc(); //{ return this->m_description; }

		D3D11_SHADER_VARIABLE_DESC GetVarDesc(); // {return this->}
		D3D11_SHADER_TYPE_DESC  GetTypeDesc();

		int IsValid() { return (m_pShader != nullptr); }
		int IsSubtype() { return m_is_subtype; }

	private:
		Renderer & m_rRender;
		Shader * m_pShader;

		int m_is_subtype;
		size_t m_id, m_vid;

	};

	class ShaderResourceManager {
		// ... 
	public:

	};

	// ================================================================================================================================
	__declspec(align(16)) class Shader : virtual public Referencable, public AlignedNew<Shader>
	// class Shader : virtual public Referencable
	{

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

		// ----
		// access constant buffers and variables 
		size_t GetParamCount() { return this->m_vCBuffers.size(); }
		size_t GetParamCount(size_t id) { this->m_vCBuffers[id].m_vConstVars.size(); } ///@todo bounds check

		///@todo ezeblol csinaljon valaki operator[]-t

		inline ShaderParamManager GetParam(Renderer &render, const char* name) { return GetParam(render, std::string(name)); }
		inline ShaderParamManager GetParam(Renderer &render, std::string name);
		inline ShaderParamManager GetParam(Renderer &render, size_t id);
									
		inline ShaderParamManager GetParam(Renderer &render, const char* name, const char* varname) { return GetParam(render, std::string(name), std::string(name)); }
		inline ShaderParamManager GetParam(Renderer &render, std::string name, std::string varname);
		inline ShaderParamManager GetParam(Renderer &render, size_t id, const char* varname) { return GetParam(render, id, std::string(varname)); }
		inline ShaderParamManager GetParam(Renderer &render, size_t id, std::string varname);
		inline ShaderParamManager GetParam(Renderer &render, size_t id, size_t vid);

		inline void* MapParamBuffer(Renderer& render, size_t id);
		inline void* GetMappedPtr(size_t id);
		inline void UnMapParamBuffer(Renderer& render, size_t id);
		
		inline D3D11_MAPPED_SUBRESOURCE GetCBMappedResourceDesc(size_t id);
		inline D3D11_SHADER_BUFFER_DESC GetCBDescription(size_t id);
		
		inline D3D11_SHADER_VARIABLE_DESC GetCBVariableDescriptor(size_t id, size_t vid);
		inline D3D11_SHADER_TYPE_DESC GetCBTypeDescriptor(size_t id, size_t vid);

		// ----
		// access bounded resources
		size_t GetBResCount() { return this->m_vBResources.size(); }
		ShaderParamManager GetBRes(Renderer &render, const char * name) { return GetBRes(render, std::string(name)); }
		ShaderParamManager GetBRes(Renderer &render, std::string name);
		ShaderParamManager GetBRes(Renderer &render, size_t id);

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

		// 

	protected:
		ID3D11ShaderReflection *m_pReflector;

		// -- constant buffer
		// Constant buffer variable 
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
			UINT m_slot;

			cb_variableMap_t m_mapConstVars;
			std::vector<CBVar> m_vConstVars;
		};

		typedef std::map<std::string, size_t> CBMap_t;
		typedef CBMap_t::iterator CBMap_it_t;

		CBMap_t m_mapCBuffers;
		std::vector<CBRecord> m_vCBuffers;

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
		std::vector<BResRecord> m_vBResources;

		// -- output sampler

	public:
		// ==========================================================================================
		/**
		A class that contains and manages one entire constant buffer with its elements from
		the reflection record for a certain constant buffer and aids to set variables into it
		*/

#if 0
		class ConstantBufferRecord
		{
		friend class Shader;
		friend class ConstantBufferElement;

		public:
			// This class has to have a public accessable default constructor due to std::map
			ConstantBufferRecord();
			~ConstantBufferRecord();

		protected:
			ConstantBufferRecord(ID3D11Device* device, ID3D11ShaderReflectionConstantBuffer * pConstantBuffer);

		public:
			void Set(const void* const pData);
			void Set(const void* const pData, size_t offset, size_t width);

			size_t GetElementCount() { return this->m_vConstVars.size(); }
			ConstantBufferElement& operator[](const char* name);
			ConstantBufferElement& operator[](size_t id);

			D3D11_SHADER_BUFFER_DESC& const GetBufferDesc() { return this->m_description; }

		protected:
			void Map();
			void Unmap();
			void *GetMappedPtr();
		private:
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
			~ConstantBufferElement();

		protected:
			ConstantBufferElement(Shader::ConstantBufferRecord* parent_record, ID3D11ShaderReflectionVariable* shader_variable);

		public:
			/// @todo implement 
			void operator= (float v) { set(&v); }

			void operator= (const float3 &v) { set(&v); }
			void operator= (const float2 &v) { set(&v); }
			void operator= (const float4 &v) { set(&v); }
			void operator= (const matrix &v) { set(&v); }

			/// @todo 16-os alignmentet meg kell szerelni
			//void operator= (Grafkit::Matrix v);

			void set(const void * const v);

			void set(const float v0);
			void set(const float v0, const float v1);
			void set(const float v0, const float v1, const float v2);
			void set(const float v0, const float v1, const float v2, const float v3);

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
#endif
	};

	// ================================================================================================================================

	typedef Ref<Shader> ShaderRef_t;

#define SHADER_BUCKET "shader"

	/**
	enhance Reference with operator [] to acces the shader's indides, avoiding dereferencing
	- itt most osszevontam az assettel az egeszet, remelem nem lesz miserable failure az egesz
	*/
	class ShaderRes : public ShaderRef_t, public Grafkit::IResource {
		friend class ShaderResRef;

	public:
		// Shader::ConstantBufferRecord& operator[](const char *name) { return this->ptr->operator[](name); }
		// Shader::ConstantBufferRecord& operator[](size_t id) { return this->ptr->operator[](id); }

		ShaderRes() : IResource() {}
		ShaderRes(Shader* ptr) : IResource(), ShaderRef_t(ptr) {}
		ShaderRes(Ref<Shader> ptr) : IResource(), ShaderRef_t(ptr) {}

		ShaderRes& operator = (Shader* in_ptr) { this->AssingnRef(in_ptr); return *this; }
		ShaderRes& operator = (ShaderRes &in_ptr) { this->AssingnRef(in_ptr); return *this; }

		~ShaderRes() {}
		virtual const char* GetBucketID() { return SHADER_BUCKET; }
	};

	typedef ShaderRes ShaderRef;

	typedef Ref<ShaderRes> ShaderAssetRef_t;

	class ShaderResRef : public ShaderAssetRef_t
	{
	public:
		ShaderResRef(): ShaderAssetRef_t(){}
		ShaderResRef(ShaderRes *ptr) : ShaderAssetRef_t(ptr) {}
		ShaderResRef(ShaderResRef& other) : ShaderAssetRef_t(other) {}

		~ShaderResRef() {}

		///@todo release modban makrozza ki az exception dobast
		operator Shader* () { 
			if (!ptr->ptr)
				throw EX_DETAILS(FWdebugExceptions::NullPointerException, this->ptr->GetName().c_str());

			return this->ptr->ptr; 
		}
		
		operator ShaderRef () { 
			if (!ptr->ptr)
				throw EX_DETAILS(FWdebugExceptions::NullPointerException, this->ptr->GetName().c_str());

			return this->ptr->ptr; 
		}
		ShaderResRef& operator = (Shader* in_ptr) { this->ptr->AssingnRef(in_ptr); return *this; }
		ShaderResRef& operator = (ShaderRef &in_ptr) { this->ptr->AssingnRef(in_ptr); return *this; }
		ShaderResRef& operator = (ShaderResRef& other) { this->AssingnRef(other); return *this; }
	};

	// ================================================================================================================================

}

// Definition of exceptions

DEFINE_EXCEPTION(MissingShaderException, 1100, "Missing shader file")

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

#define _HAS_ShaderException

namespace FWdebugExceptions {
	class ShaderException {
		// ... 

	};
}

#endif // NO_ShaderException

