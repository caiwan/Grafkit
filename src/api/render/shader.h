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

	// ================================================================================================================================
	class Shader : virtual public Referencable
	{
	public:
		Shader();
		~Shader();

		void LoadFromFile(Renderer & device, LPCSTR entry, LPCWCHAR file, ID3DInclude* pInclude = nullptr, D3D_SHADER_MACRO* pDefines = nullptr);
		void LoadFromMemory(Renderer & device, LPCSTR entry, LPCSTR source, size_t size, LPCSTR name, ID3DInclude* pInclude = nullptr, D3D_SHADER_MACRO* pDefines = nullptr);

		void Shutdown();

		void Bind(ID3D11DeviceContext* deviceContext);
		void Unbind(ID3D11DeviceContext* deviceContext);

		virtual enum ShaderType_e GetShaderType() = 0;

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
		size_t GetParamValueCount(size_t id) { return id>= GetParamCount() ? 0:this->m_cBuffers[id].m_cbVars.size(); }

		void SetParam(ID3D11DeviceContext * deviceContext, std::string name, const void * const pData, size_t size = 0, size_t offset = 0);
		void SetParam(ID3D11DeviceContext * deviceContext, size_t id, const void * const pData, size_t size = 0, size_t offset = 0);

		template <typename T > void SetParamT(ID3D11DeviceContext * deviceContext, std::string name, T v) { SetParam(deviceContext, name, &v); }

		void SetParamValue(ID3D11DeviceContext * deviceContext, std::string name, std::string valueName, void * const pData, size_t size = 0, size_t offset = 0);
		void SetParamValue(ID3D11DeviceContext * deviceContext, size_t id, size_t valueid, const void * const pData, size_t size = 0, size_t offset = 0);

		template <typename T > void SetParamValueT(ID3D11DeviceContext * deviceContext, std::string name, std::string valueName, T v) { SetParamValue(deviceContext, name, valueName, &v, sizeof(T)); }


	protected:
		void* MapParamBuffer(ID3D11DeviceContext * deviceContext, size_t id, int isDiscard = 1);
		void* GetMappedPtr(size_t id);
		void UnMapParamBuffer(ID3D11DeviceContext * deviceContext, size_t id);

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
		// set bounded resources
		size_t GetBoundedResourceCount() { return this->m_bResources.size(); }
		D3D11_SHADER_INPUT_BIND_DESC GetBoundedResourceDesc(size_t id) { return m_bResources[id].m_desc; }
		
		void SetSamplerSatate(ID3D11DeviceContext * deviceContext, std::string name, ID3D11SamplerState * pSampler) { SetBoundedResourcePointer(deviceContext, name, pSampler); }
		void SetSamplerSatate(ID3D11DeviceContext * deviceContext, size_t id, ID3D11SamplerState * pSampler) { SetBoundedResourcePointer(deviceContext, id, pSampler); }

		void SetShaderResourceView(ID3D11DeviceContext * deviceContext, std::string name, ID3D11ShaderResourceView* pResView) { SetBoundedResourcePointer(deviceContext, name, pResView); }
		void SetShaderResourceView(ID3D11DeviceContext * deviceContext, size_t id, ID3D11ShaderResourceView* pResView) { SetBoundedResourcePointer(deviceContext, id, pResView); }

		void SetBoundedResourcePointer(ID3D11DeviceContext * deviceContext, std::string name, void* ptr);
		void SetBoundedResourcePointer(ID3D11DeviceContext * deviceContext, size_t id, void* ptr);

		// ----
		// access input layout 
		size_t GetILayoutElemCount() { return this->m_mapInputElems.size(); }
		InputElementRecord GetILayoutElem(size_t index) { return this->m_mapInputElems[index]; }

	private:
		void DispatchShaderErrorMessage(ID3D10Blob* errorMessage, LPCWCHAR file, LPCSTR entry);
		void GetDXGIFormat(D3D11_SIGNATURE_PARAMETER_DESC pd, DXGI_FORMAT &res, UINT &byteWidth);
		void BuildReflection(Renderer & device, ID3D10Blob* shaderBuffer);

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

		std::map<std::string, size_t> m_mapCBuffers;
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

		std::map<std::string, size_t> m_mapBResources;
		
		std::vector<BResRecord> m_bResources;

		// -- output sampler
		std::vector<OutputTargetRecord> m_outputTargets;

		// ================================================================================================================================

		// pure virtuals for shader type specific stuff (vs, fs, etc..)
		protected:
			virtual void ShutdownChild() = 0;
			
			virtual HRESULT CompileShaderFromFile(LPCWCHAR file, D3D_SHADER_MACRO* pDefines, ID3DInclude* pInclude, LPCSTR entry, ID3D10Blob*& shaderBuffer, ID3D10Blob*& errorMessage) = 0;
			virtual HRESULT CompileShaderFromSource(LPCSTR source, size_t size, LPCSTR sourceName, D3D_SHADER_MACRO* pDefines, ID3DInclude* pInclude, LPCSTR entry, ID3D10Blob*& shaderBuffer, ID3D10Blob*& errorMessage) = 0;
			
			virtual void CreateShader(ID3D11Device* device, ID3D10Blob* shaderBuffer, ID3D11ClassLinkage *pClassLinkage = nullptr) = 0;

			virtual void SetConstantBuffer(ID3D11DeviceContext * deviceContext, UINT slot, UINT numBuffers, ID3D11Buffer*& buffer) = 0;
			virtual void SetShaderResources(ID3D11DeviceContext * deviceContext, UINT bindPoint, UINT bindCount, ID3D11ShaderResourceView *& pResView) = 0;
			virtual void SetSamplerPtr(ID3D11DeviceContext* device, UINT bindPoint, UINT bindCount, ID3D11SamplerState *& pSampler) = 0;

			virtual void BindShader(ID3D11DeviceContext * deviceContext) = 0;
			virtual void UnbindShader(ID3D11DeviceContext * deviceContext) = 0;
	};

	// ================================================================================================================================

	typedef Ref<Shader> ShaderRef;
	typedef Grafkit::Resource<Shader> ShaderRes;
	typedef Ref<ShaderRes> ShaderResRef;

	enum ShaderType_e {
		SHADER_NONE = 0,

		SHADER_VERTEX,
		SHADER_PIXEL,
		SHADER_GEOMETRY,
		SHADER_COMPUTE,

		SHADER_TYPE_COUNT
	};

	/*
	* VertexShader
	*/
	class VertexShader : public Shader {
	public:
		VertexShader();
		~VertexShader();

		enum ShaderType_e GetShaderType() { return SHADER_VERTEX; }

	protected:
		void ShutdownChild();
		HRESULT CompileShaderFromFile(LPCWCHAR file, D3D_SHADER_MACRO* pDefines, ID3DInclude* pInclude, LPCSTR entry, ID3D10Blob*& shaderBuffer, ID3D10Blob*& errorMessage);
		HRESULT CompileShaderFromSource(LPCSTR source, size_t size, LPCSTR sourceName, D3D_SHADER_MACRO* pDefines, ID3DInclude* pInclude, LPCSTR entry, ID3D10Blob*& shaderBuffer, ID3D10Blob*& errorMessage);
		void CreateShader(ID3D11Device* device, ID3D10Blob* shaderBuffer, ID3D11ClassLinkage *pClassLinkage = nullptr);
		
		void SetSamplerPtr(ID3D11DeviceContext* device,UINT bindPoint, UINT bindCount, ID3D11SamplerState *& pSampler);
		void SetShaderResources(ID3D11DeviceContext * deviceContext, UINT bindPoint, UINT bindCount, ID3D11ShaderResourceView *& pResView);
		void SetConstantBuffer(ID3D11DeviceContext * deviceContext, UINT slot, UINT numBuffers, ID3D11Buffer*& buffer);

		void BindShader(ID3D11DeviceContext * deviceContext);
		void UnbindShader(ID3D11DeviceContext * deviceContext);
		

	private:
		ID3D11VertexShader* m_vxShader;
	};

	/*
	* Pixel shader
	*/
	class PixelShader : public Shader {
	public:
		PixelShader();
		~PixelShader();

		enum ShaderType_e GetShaderType() { return SHADER_PIXEL; }

	protected:
		void ShutdownChild();
		HRESULT CompileShaderFromFile(LPCWCHAR file, D3D_SHADER_MACRO* pDefines, ID3DInclude* pInclude, LPCSTR entry, ID3D10Blob*& shaderBuffer, ID3D10Blob*& errorMessage);
		HRESULT CompileShaderFromSource(LPCSTR source, size_t size, LPCSTR sourceName, D3D_SHADER_MACRO* pDefines, ID3DInclude* pInclude, LPCSTR entry, ID3D10Blob*& shaderBuffer, ID3D10Blob*& errorMessage);
		void CreateShader(ID3D11Device* device, ID3D10Blob* shaderBuffer, ID3D11ClassLinkage *pClassLinkage = nullptr);

		void SetSamplerPtr(ID3D11DeviceContext* device, UINT bindPoint, UINT bindCount, ID3D11SamplerState *& pSampler);
		void SetShaderResources(ID3D11DeviceContext * deviceContext, UINT bindPoint, UINT bindCount, ID3D11ShaderResourceView *& pResView);
		void SetConstantBuffer(ID3D11DeviceContext * deviceContext, UINT slot, UINT numBuffers, ID3D11Buffer*& buffer);

		void BindShader(ID3D11DeviceContext * deviceContext);
		void UnbindShader(ID3D11DeviceContext * deviceContext);
	private:
		ID3D11PixelShader* m_pxShader;
	};

	/*
	* Pixel shader
	*/
	class GeometryShader : public Shader {
	public:
		GeometryShader();
		~GeometryShader();

		enum ShaderType_e GetShaderType() { return SHADER_GEOMETRY; }

	protected:
		void ShutdownChild();
		HRESULT CompileShaderFromFile(LPCWCHAR file, D3D_SHADER_MACRO* pDefines, ID3DInclude* pInclude, LPCSTR entry, ID3D10Blob*& shaderBuffer, ID3D10Blob*& errorMessage);
		HRESULT CompileShaderFromSource(LPCSTR source, size_t size, LPCSTR sourceName, D3D_SHADER_MACRO* pDefines, ID3DInclude* pInclude, LPCSTR entry, ID3D10Blob*& shaderBuffer, ID3D10Blob*& errorMessage);
		void CreateShader(ID3D11Device* device, ID3D10Blob* shaderBuffer, ID3D11ClassLinkage *pClassLinkage = nullptr);

		void SetSamplerPtr(ID3D11DeviceContext* device, UINT bindPoint, UINT bindCount, ID3D11SamplerState *& pSampler);
		void SetShaderResources(ID3D11DeviceContext * deviceContext, UINT bindPoint, UINT bindCount, ID3D11ShaderResourceView *& pResView);
		void SetConstantBuffer(ID3D11DeviceContext * deviceContext, UINT slot, UINT numBuffers, ID3D11Buffer*& buffer);

		void BindShader(ID3D11DeviceContext * deviceContext);
		void UnbindShader(ID3D11DeviceContext * deviceContext);

	private:
		ID3D11GeometryShader* m_gmShader;
	};

//@Todo compute shader

}

// Definition of exceptions

#define EX_ERROR_SHADER 1100

DEFINE_EXCEPTION(MissingShaderException, EX_ERROR_SHADER + 0, "Missing shader file")

///@todo ezeket at kell pakolni a shader exceptionbe majd
DEFINE_EXCEPTION(VSCrerateException, EX_ERROR_SHADER + 1, "Could not create vertex shader")
DEFINE_EXCEPTION(PSCrerateException, EX_ERROR_SHADER + 2, "Could not create pixel shader")
DEFINE_EXCEPTION(GSCrerateException, EX_ERROR_SHADER + 3, "Could not create geometry shader")

DEFINE_EXCEPTION(InputLayoutCreateException, EX_ERROR_SHADER + 10, "Could not create input layout")
DEFINE_EXCEPTION(ConstantBufferCreateException, EX_ERROR_SHADER + 11, "Could not create constant buffer")
DEFINE_EXCEPTION(ConstantBufferLocateException, EX_ERROR_SHADER + 12, "Could not locate constant buffer")
DEFINE_EXCEPTION(ConstantBufferMapException, EX_ERROR_SHADER + 13, "Could not map constant buffer")
DEFINE_EXCEPTION(BoundResourceLocateException, EX_ERROR_SHADER + 14, "Could not locate bound resouce")

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

