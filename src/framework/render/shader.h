#pragma once

#include "common.h"
#include <d3d11shader.h>

#include "renderer.h"


namespace Grafkit
{
    // ================================================================================================================================
    __declspec(align(16))  class Shader : public AlignedNew<Shader>
    {
    public:
        Shader();

        Shader(const Shader& other) = delete;
        Shader& operator=(const Shader& other) = delete;

        void LoadFromFile(Renderer& device, LPCSTR entry, LPCWCHAR file, const std::unique_ptr<ID3DInclude>& pInclude = nullptr, const std::shared_ptr<D3D_SHADER_MACRO> &pDefines = nullptr);
        void LoadFromMemory(Renderer& device, LPCSTR entry, LPCSTR source, size_t size, LPCSTR name, const std::unique_ptr<ID3DInclude>& pInclude = nullptr, const std::shared_ptr<D3D_SHADER_MACRO> &pDefines = nullptr);

        void Bind(Renderer & render);
        void Unbind(Renderer & render);

        virtual enum ShaderType_e GetShaderType() = 0;

        std::string GetName() const { return m_name; }
        void SetName(const std::string& name) { m_name = name; }

    private:
        void CompileShader(Renderer& device, ComPtr<ID3D10Blob> &outShaderBuffer);
        std::string m_name;
    public:

        // ==========================================================================================

        /**
        A struct that extends the input layout descriptor
        */
        struct InputElementRecord
        {
            D3D11_INPUT_ELEMENT_DESC desc;
            uint32_t width;
            uint32_t offset;
            std::string name;
        };

        struct OutputTargetRecord
        {
            D3D11_SIGNATURE_PARAMETER_DESC desc;
        };

        // ----
        // access constant buffers and variables 

        // reflection?
        size_t GetParamCount() const { return this->m_cBuffers.size(); }
        size_t GetParamValueCount(uint32_t id) { return id >= GetParamCount() ? 0 : this->m_cBuffers[id].m_cbVars.size(); }

        // get direct id, to spped it up a bit 
        int GetParamId(const std::string &name);

        void SetParam(Renderer & render, const std::string &name, const void* const &pData, size_t size = 0, size_t offset = 0);
        void SetParam(Renderer & render, uint32_t id, const void* const &pData, size_t size = 0, size_t offset = 0);

        template <typename T>
        void SetParamT(Renderer & render, const std::string &name, T& v) { SetParam(render, name, &v); }

        template <typename T>
        void SetParamT(Renderer & render, uint32_t id, T& v) { SetParam(render, id, &v); }

        // we don't really need much of these
        void SetParamValue(Renderer & render, const std::string &name, const std::string &valueName, const void* const &pData, size_t size = 0, size_t offset = 0);
        void SetParamValue(Renderer & render, uint32_t id, size_t valueid, const void* const &pData, size_t size = 0, size_t offset = 0);

        // and this one 
        template <typename T>
        void SetParamValueT(Renderer & render, const std::string &name, const std::string &valueName, const T& v) { SetParamValue(render, name, valueName, &v, sizeof(T)); }

    protected:
        void* MapParamBuffer(Renderer & render, uint32_t id, int isDiscard = 1);
        void* GetMappedPtr(uint32_t id);
        void UnMapParamBuffer(Renderer & render, uint32_t id);

    public:

        D3D11_SHADER_BUFFER_DESC GetCBDescription(uint32_t id) { return m_cBuffers[id].m_description; }

        D3D11_SHADER_VARIABLE_DESC GetCBVariableDescriptor(uint32_t id, size_t vid) { return m_cBuffers[id].m_cbVars[vid].m_var_desc; }
        D3D11_SHADER_TYPE_DESC GetCBTypeDescriptor(uint32_t id, size_t vid) { return m_cBuffers[id].m_cbVars[vid].m_type_desc; }

        // ----
        // set bounded resources
        size_t GetBoundedResourceCount() const { return this->m_bResources.size(); }
        D3D11_SHADER_INPUT_BIND_DESC GetBoundedResourceDesc(uint32_t id) { return m_bResources[id].m_desc; }

        int GetBoundedResourceId(const std::string &name);

        void SetSamplerSatate(Renderer & render, const std::string &name, const ComPtr<ID3D11SamplerState> &pSampler) { SetBoundedResourcePointer(render, name, pSampler.Get()); }
        void SetSamplerSatate(Renderer & render, uint32_t id, const ComPtr<ID3D11SamplerState>& pSampler) { SetBoundedResourcePointer(render, id, pSampler.Get()); }

        void SetShaderResourceView(Renderer & render, const std::string &name, const ComPtr<ID3D11ShaderResourceView>& pResView) { SetBoundedResourcePointer(render, name, pResView.Get()); }
        void SetShaderResourceView(Renderer & render, uint32_t id, const ComPtr<ID3D11ShaderResourceView>& pResView) { SetBoundedResourcePointer(render, id, pResView.Get()); }

        void SetBoundedResourcePointer(Renderer & render, const std::string &name, const void* const &ptr);
        void SetBoundedResourcePointer(Renderer & render, uint32_t id, const void* const &ptr);

    private:
        void SetBoundedResource(Renderer & render, uint32_t id, const void* const &ptr);

    public:
        // ----
        // access input layout 
        size_t GetILayoutElemCount() const { return this->m_inputElems.size(); }
        InputElementRecord GetILayoutElem(uint32_t index) { return this->m_inputElems[index]; }

    private:
        static void DispatchShaderErrorMessage(const ComPtr<ID3D10Blob>& errorMessage, LPCWCHAR file, LPCSTR entry);
        static void GetDXGIFormat(D3D11_SIGNATURE_PARAMETER_DESC pd, DXGI_FORMAT& res, uint32_t& byteWidth);
        void BuildReflection(Renderer& device, const ComPtr<ID3D10Blob>& shaderBuffer);

    protected:
        ComPtr<ID3D11ShaderReflection> m_pReflector;

        // -- constant buffer
        // Constant buffer variable 

        ///@todo a kovetkezo iteracioban nem kell reflektalni teljesen a shadert, eleg csak a buffereket letrehozni CPU oldalon

        struct CBVar
        {
            D3D11_SHADER_VARIABLE_DESC m_var_desc;
            D3D11_SHADER_TYPE_DESC m_type_desc;
        };

        typedef std::map<std::string, uint32_t> cb_variableMap_t;
        typedef cb_variableMap_t::iterator cb_variableMap_it;

        // constant buffer record 
        struct CBRecord
        {
            D3D11_MAPPED_SUBRESOURCE m_mappedResource;
            D3D11_SHADER_BUFFER_DESC m_description;
            ComPtr<ID3D11Buffer> m_buffer;
            std::unique_ptr<uint8_t> m_cpuBuffer;
            uint32_t m_slot;

            cb_variableMap_t m_cbVarMap;
            std::vector<CBVar> m_cbVars;
        };

        std::map<std::string, int> m_mapCBuffers;
        std::vector<CBRecord> m_cBuffers;

        // -- input layout 
        typedef std::vector<InputElementRecord> inputElements_t;
        inputElements_t m_inputElems;
        ID3D11InputLayout* m_layout;

        // -- bounded resource

        struct BResRecord
        {
            D3D11_SHADER_INPUT_BIND_DESC m_desc;
            void* m_boundSource;
        };

        std::map<std::string, size_t> m_mapBResources;

        std::vector<BResRecord> m_bResources;

        // -- output sampler
        std::vector<OutputTargetRecord> m_outputTargets;

        // 
        int m_isBound;

        // ================================================================================================================================

        // pure virtuals for shader type specific stuff (vs, fs, etc..)
        virtual HRESULT CompileShaderFromFile(LPCWCHAR file, const std::shared_ptr<D3D_SHADER_MACRO>& pDefines, const std::unique_ptr<ID3DInclude>& pInclude, LPCSTR entry, ComPtr<ID3D10Blob>& outShaderBuffer, ComPtr<ID3D10Blob>& outErrorMessage) = 0;
        virtual HRESULT CompileShaderFromSource(LPCSTR source, size_t size, LPCSTR sourceName, const std::shared_ptr<D3D_SHADER_MACRO> &pDefines, const std::unique_ptr<ID3DInclude>& pInclude, LPCSTR entry, ComPtr<ID3D10Blob>& outShaderBuffer, ComPtr<ID3D10Blob>& outErrorMessage) = 0;

        virtual void CreateShader(Renderer &render, ComPtr<ID3D10Blob> outShaderBuffer, const ComPtr<ID3D11ClassLinkage> & pClassLinkage = nullptr) = 0;

        virtual void SetConstantBuffer(Renderer & render, uint32_t slot, uint32_t numBuffers, const ComPtr<ID3D11Buffer>& buffer) = 0;
        virtual void SetShaderResources(Renderer & render, uint32_t bindPoint, uint32_t bindCount, const ComPtr<ID3D11ShaderResourceView>& pResView) = 0;
        virtual void SetSamplerPtr(Renderer &render, uint32_t bindPoint, uint32_t bindCount, const ComPtr<ID3D11SamplerState>& pSampler) = 0;

        virtual void BindShader(Renderer & render) = 0;
        virtual void UnbindShader(Renderer & render) = 0;
    };

    // ================================================================================================================================

    enum ShaderType_e
    {
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
    class VertexShader : public Shader
    {
    public:
        enum ShaderType_e GetShaderType() override { return SHADER_VERTEX; }

    protected:
        HRESULT CompileShaderFromFile(LPCWCHAR file, const std::shared_ptr<D3D_SHADER_MACRO>& pDefines, const std::unique_ptr<ID3DInclude>& pInclude, LPCSTR entry, ComPtr<ID3D10Blob>& outShaderBuffer, ComPtr<ID3D10Blob>& outErrorMessage) override;
        HRESULT CompileShaderFromSource(LPCSTR source, size_t size, LPCSTR sourceName, const std::shared_ptr<D3D_SHADER_MACRO> &pDefines, const std::unique_ptr<ID3DInclude>& pInclude, LPCSTR entry, ComPtr<ID3D10Blob>& outShaderBuffer, ComPtr<ID3D10Blob>& outErrorMessage) override;
        void CreateShader(Renderer &render, ComPtr<ID3D10Blob> outShaderBuffer, const ComPtr<ID3D11ClassLinkage>& pClassLinkage) override;
        
        void SetConstantBuffer(Renderer & render, uint32_t slot, uint32_t numBuffers, const ComPtr<ID3D11Buffer>& buffer) override;
        void SetShaderResources(Renderer & render, uint32_t bindPoint, uint32_t bindCount, const ComPtr<ID3D11ShaderResourceView>& pResView) override;
        void SetSamplerPtr(Renderer &render, uint32_t bindPoint, uint32_t bindCount, const ComPtr<ID3D11SamplerState>& pSampler) override;
        
        void BindShader(Renderer & render) override;
        void UnbindShader(Renderer & render) override;
    private:
        ComPtr<ID3D11VertexShader> m_vxShader;
    };

    /*
    * Pixel shader
    */
    class PixelShader : public Shader
    {
    public:
        enum ShaderType_e GetShaderType() override { return SHADER_PIXEL; }

    protected:
        HRESULT CompileShaderFromFile(LPCWCHAR file, const std::shared_ptr<D3D_SHADER_MACRO>& pDefines, const std::unique_ptr<ID3DInclude>& pInclude, LPCSTR entry, ComPtr<ID3D10Blob>& outShaderBuffer, ComPtr<ID3D10Blob>& outErrorMessage) override;
        HRESULT CompileShaderFromSource(LPCSTR source, size_t size, LPCSTR sourceName, const std::shared_ptr<D3D_SHADER_MACRO>& pDefines, const std::unique_ptr<ID3DInclude>& pInclude, LPCSTR entry, ComPtr<ID3D10Blob>& outShaderBuffer, ComPtr<ID3D10Blob>& outErrorMessage) override;
        void CreateShader(Renderer &render, ComPtr<ID3D10Blob> outShaderBuffer, const ComPtr<ID3D11ClassLinkage>& pClassLinkage) override;

        void SetConstantBuffer(Renderer & render, uint32_t slot, uint32_t numBuffers, const ComPtr<ID3D11Buffer>& buffer) override;
        void SetShaderResources(Renderer & render, uint32_t bindPoint, uint32_t bindCount, const ComPtr<ID3D11ShaderResourceView>& pResView) override;
        void SetSamplerPtr(Renderer& render, uint32_t bindPoint, uint32_t bindCount, const ComPtr<ID3D11SamplerState>& pSampler) override;

        void BindShader(Renderer & render) override;
        void UnbindShader(Renderer & render) override;
    private:
        ComPtr<ID3D11PixelShader> m_pxShader;
    };

    /*
    * Pixel shader
    */
    class GeometryShader : public Shader
    {
    public:
        enum ShaderType_e GetShaderType() override { return SHADER_GEOMETRY; }

    protected:
        HRESULT CompileShaderFromFile(LPCWCHAR file, const std::shared_ptr<D3D_SHADER_MACRO>& pDefines, const std::unique_ptr<ID3DInclude>& pInclude, LPCSTR entry, ComPtr<ID3D10Blob>& outShaderBuffer, ComPtr<ID3D10Blob>& outErrorMessage) override;
        HRESULT CompileShaderFromSource(LPCSTR source, size_t size, LPCSTR sourceName, const std::shared_ptr<D3D_SHADER_MACRO>& pDefines, const std::unique_ptr<ID3DInclude>& pInclude, LPCSTR entry, ComPtr<ID3D10Blob>& outShaderBuffer, ComPtr<ID3D10Blob>& outErrorMessage) override;
        void CreateShader(Renderer &render, ComPtr<ID3D10Blob> outShaderBuffer, const ComPtr<ID3D11ClassLinkage>& pClassLinkage) override;

        void SetConstantBuffer(Renderer & render, uint32_t slot, uint32_t numBuffers, const ComPtr<ID3D11Buffer>& buffer) override;
        void SetShaderResources(Renderer & render, uint32_t bindPoint, uint32_t bindCount, const ComPtr<ID3D11ShaderResourceView>& pResView) override;

        void SetSamplerPtr(Renderer& render, uint32_t bindPoint, uint32_t bindCount, const ComPtr<ID3D11SamplerState>& pSampler) override;

        void BindShader(Renderer & render) override;
        void UnbindShader(Renderer & render) override;

    private:
        ComPtr<ID3D11GeometryShader> m_gmShader;
    };

    //@Todo compute shader
}
