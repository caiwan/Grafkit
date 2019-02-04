#define INITGUID 

#include "stdafx.h"

#include <string>

#include <cstdio>

#include "shader.h"

using namespace Grafkit;


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

DEFINE_EXCEPTION(ShaderException, 1200, "Could not compile shader")

using FWdebug::Exception;
using namespace FWdebugExceptions;

#define PS_VERSION "ps_4_0"
#define VS_VERSION "vs_4_0"
#define GS_VERSION "gs_4_0"
#define CS_VERSION "cs_4_0"

#ifdef _DEBUG
#define SHADER_COMPILE_FLAGS (D3DCOMPILE_DEBUG)
#else //DEBUG
#define SHADER_COMPILE_FLAGS (0)
#endif // DEBUG


// TODO http://stackoverflow.com/questions/24323281/the-pixel-shader-unit-expects-a-sampler

// =============================================================================================================================

Shader::Shader() :
    m_pReflector(nullptr)
    , m_layout(nullptr)
    , m_isBound(0) {
}

void Shader::LoadFromFile(Renderer& device, LPCSTR entry, LPCWCHAR file, const std::unique_ptr<ID3DInclude>& pInclude, const std::shared_ptr<D3D_SHADER_MACRO>& pDefines)
{
    HRESULT result = 0;
    ComPtr<ID3D10Blob> errorMessage = nullptr;
    ComPtr<ID3D10Blob> shaderBuffer = nullptr;
    unsigned int numElements = 0;

    // input checking
    if (!entry) { throw std::runtime_error("No shader entrypoint was given"); }
    if (!file) { throw std::runtime_error("No shader filename was given"); }

    result = CompileShaderFromFile(file, pDefines, pInclude, entry, shaderBuffer, errorMessage);

    if (FAILED(result))
    {
        // If the shader failed to compile it should have writen something to the error message.
        if (errorMessage)
            DispatchShaderErrorMessage(errorMessage, file, entry);
        // If there was nothing in the error message then it simply could not find the shader file itself.
        else
        THROW_EX_HRESULT(MissingShaderException, result);
    }

    this->CompileShader(device, shaderBuffer);
}


void Shader::LoadFromMemory(Renderer& device, LPCSTR entry, LPCSTR source, size_t size, LPCSTR name, const std::unique_ptr<ID3DInclude>& pInclude, const std::shared_ptr<D3D_SHADER_MACRO>& pDefines)
{
    HRESULT result = 0;
    ComPtr<ID3D10Blob> errorMessage = nullptr;
    ComPtr<ID3D10Blob> shaderBuffer = nullptr;

    // input checking
    if (!entry) { throw std::runtime_error("No shader entrypoint was given"); }
    if (!source) { throw std::runtime_error("No shader source was given"); }

    m_name = name;

    result = CompileShaderFromSource(source, size, name, pDefines, pInclude, entry, shaderBuffer, errorMessage);

    if (FAILED(result))
    {
        // If the shader failed to compile it should have writen something to the error message.
        if (errorMessage) { DispatchShaderErrorMessage(errorMessage, nullptr, entry); }
        // If there was nothing in the error message then it simply could not find the shader file itself.
        else
        {
            THROW_EX(MissingShaderException);
        }
    }

    this->CompileShader(device, shaderBuffer);
}

void Shader::Bind(Renderer& render)
{
    BindShader(render);

    m_isBound = 1;

    // duck the constant buffers around
    if (this->GetParamCount())
    {
        for (size_t i = 0; i < this->GetParamCount(); i++)
        {
            ID3D11Buffer* buffer = this->m_cBuffers[i].m_buffer.Get();
            uint32_t slot = this->m_cBuffers[i].m_slot;

            SetConstantBuffer(render, slot, 1, buffer);
        }
    }

    if (this->GetBoundedResourceCount())
    {
        for (size_t i = 0; i < this->GetBoundedResourceCount(); i++)
        {
            BResRecord& brRecord = this->m_bResources[i];
            SetBoundedResource(render, i, brRecord.m_boundSource);
        }
    }
}

void Shader::Unbind(Renderer& render)
{
    if (this->GetParamCount())
    {
        for (size_t i = 0; i < this->GetParamCount(); i++)
        {
            ID3D11Buffer* buffer = nullptr;
            uint32_t slot = this->m_cBuffers[i].m_slot;
            SetConstantBuffer(render, slot, 1, buffer);
        }
    }

    if (this->GetBoundedResourceCount()) { for (size_t i = 0; i < this->GetBoundedResourceCount(); i++) { SetBoundedResource(render, i, nullptr); } }

    UnbindShader(render);

    m_isBound = 0;
}

void Shader::CompileShader(Renderer& render, ComPtr<ID3D10Blob>& outShaderBuffer)
{
    LOGGER(Log::Logger().Info("- Compiling shader"));
    CreateShader(render, outShaderBuffer);
    LOGGER(Log::Logger().Info("- Compiling shader OK"));

    this->BuildReflection(render, outShaderBuffer);
}

int Shader::GetParamId(const std::string& name)
{
    auto it = this->m_mapCBuffers.find(name);
    if (it != m_mapCBuffers.end()) { return it->second; }
    return -1;
}

// ============================================================================================================================================
// set param
void Shader::SetParam(Renderer& render, const std::string& name, const void* const & pData, size_t size, size_t offset)
{
    uint32_t id = GetParamId(name);
    SetParam(render, id, pData, size, offset);
}

void Shader::SetParam(Renderer& render, uint32_t id, const void* const & pData, size_t size, size_t offset)
{
    if (id < 0 || id >= GetParamCount())
        return;

    const auto& buffer = m_cBuffers[id];
    const auto& bufferSize = buffer.m_description.Size;

    if (size == 0)
    {
        size = bufferSize;
        offset = 0;
    }

    auto ptr = buffer.m_cpuBuffer.get();

    if (size + offset <= bufferSize)
    CopyMemory(ptr + offset, pData, size);
    else
        DebugBreak();

    auto mappedPtr = static_cast<uint8_t*>(this->MapParamBuffer(render, id));

    CopyMemory(mappedPtr, ptr, bufferSize);
    this->UnMapParamBuffer(render, id);
}

// set value inside a value 
void Shader::SetParamValue(Renderer& render, const std::string& name, const std::string& valueName, const void* const & pData, size_t size, size_t offset)
{
    auto it = this->m_mapCBuffers.find(name);
    if (it != m_mapCBuffers.end())
    {
        auto vit = this->m_cBuffers[it->second].m_cbVarMap.find(valueName);
        SetParamValue(render, it->second, vit->second, pData, size, offset);
    }
}

void Shader::SetParamValue(Renderer& render, uint32_t id, size_t valueId, const void* const & pData, size_t size, size_t offset)
{
    if (id < GetParamCount() && valueId < GetParamValueCount(id))
    {
        const auto& buffer = m_cBuffers[id];
        const auto& bufferSize = buffer.m_description.Size;
        const auto& valueBuffer = buffer.m_cbVars[valueId];
        const auto& valueSize = valueBuffer.m_var_desc.Size;
        const auto& valueOffset = valueBuffer.m_var_desc.StartOffset;

        offset += valueOffset;

        if (size == 0)
            size = valueSize;

        auto ptr = buffer.m_cpuBuffer.get();

        if (size + offset <= m_cBuffers[id].m_description.Size)
        CopyMemory(ptr + offset, pData, size);
        else
            DebugBreak();

        uint8_t* mappedPtr = static_cast<uint8_t*>(this->MapParamBuffer(render, id));
        CopyMemory(mappedPtr, ptr, bufferSize);
        this->UnMapParamBuffer(render, id);
    }
    else { DebugBreak(); }
}

void* Shader::MapParamBuffer(Renderer& render, uint32_t id, int isDiscard)
{
    HRESULT result = 0;

    CBRecord& cbRecord = m_cBuffers[id];
    result = render.GetDeviceContext()->Map(cbRecord.m_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &cbRecord.m_mappedResource);
    if (FAILED(result))
    THROW_EX_HRESULT(ConstantBufferMapException, result);

    return cbRecord.m_mappedResource.pData;
}

void* Shader::GetMappedPtr(uint32_t id) { return m_cBuffers[id].m_mappedResource.pData; }

void Shader::UnMapParamBuffer(Renderer& render, uint32_t id)
{
    //if (id < m_cBufferCount)
    {
        const CBRecord& cbRecord = m_cBuffers[id];
        render.GetDeviceContext()->Unmap(cbRecord.m_buffer.Get(), 0);
    }
}


// ============================================================================================================================================

int Shader::GetBoundedResourceId(const std::string& name)
{
    auto it = this->m_mapBResources.find(name);
    if (it != m_mapBResources.end()) { return it->second; }
    return -1;
}

void Shader::SetBoundedResourcePointer(Renderer& render, const std::string& name, const void* const & ptr)
{
    auto it = this->m_mapBResources.find(name);
    if (it != this->m_mapBResources.end())
        SetBoundedResourcePointer(render, it->second, ptr);
}

void Shader::SetBoundedResourcePointer(Renderer& render, uint32_t id, const void* const & ptr)
{
    if (id >= GetBoundedResourceCount())
        return;

    this->m_bResources[id].m_boundSource = const_cast<void*>(ptr);

    if (m_isBound)
        SetBoundedResource(render, id, ptr);
}

void Shader::SetBoundedResource(Renderer& render, uint32_t id, const void* const & ptr)
{
    BResRecord& brRecord = this->m_bResources[id];

    if (brRecord.m_desc.BindCount != 1)
        DebugBreak();

    switch (brRecord.m_desc.Type)
    {
    case D3D_SIT_TEXTURE:
        {
            ID3D11ShaderResourceView* ppResV = (ID3D11ShaderResourceView*)ptr;
            SetShaderResources(render, brRecord.m_desc.BindPoint, brRecord.m_desc.BindCount, ppResV);
        }
        break;

    case D3D_SIT_SAMPLER:
        {
            ID3D11SamplerState* pSampler = (ID3D11SamplerState*)ptr;
            SetSamplerPtr(render, brRecord.m_desc.BindPoint, brRecord.m_desc.BindCount, pSampler);
        }
        break;
    }
}

void Shader::DispatchShaderErrorMessage(const ComPtr<ID3D10Blob>& errorMessage, LPCWCHAR file, LPCSTR entry)
{
    char* compileErrors = nullptr;
    unsigned long bufferSize = 0;

    compileErrors = static_cast<char*>(errorMessage->GetBufferPointer());
    bufferSize = errorMessage->GetBufferSize();

    LOGGER(Log::Logger().Error(compileErrors));

    THROW_EX_DETAILS(ShaderException, "See logfiles");
}


void Shader::GetDXGIFormat(D3D11_SIGNATURE_PARAMETER_DESC pd, DXGI_FORMAT& res, uint32_t& byteWidth)
{
    uint8_t mask = pd.Mask;
    int varCount = 0;

    while (mask)
    {
        if (mask & 0x01)
            varCount++;
        mask >>= 1;
    }

    res = DXGI_FORMAT_UNKNOWN;
    byteWidth = 0;

    ///@todo I should took this mess into a LUT
    switch (pd.ComponentType)
    {
    case D3D_REGISTER_COMPONENT_FLOAT32:
        {
            switch (varCount)
            {
            case 4:
                res = DXGI_FORMAT_R32G32B32A32_FLOAT;
                byteWidth = 4 * 4;
                return;
            case 3:
                res = DXGI_FORMAT_R32G32B32_FLOAT;
                byteWidth = 4 * 3;
                return;
            case 2:
                res = DXGI_FORMAT_R32G32_FLOAT;
                byteWidth = 4 * 2;
                return;
            case 1:
                res = DXGI_FORMAT_R32_FLOAT;
                byteWidth = 4 * 1;
            }
            break;
        }
    case D3D_REGISTER_COMPONENT_SINT32:
        {
            switch (varCount)
            {
            case 4:
                res = DXGI_FORMAT_R32G32B32A32_SINT;
                byteWidth = 2 * 4;
                return;
            case 3:
                res = DXGI_FORMAT_R32G32B32_SINT;
                byteWidth = 2 * 3;
                return;
            case 2:
                res = DXGI_FORMAT_R32G32_SINT;
                byteWidth = 2 * 2;
                return;
            case 1:
                res = DXGI_FORMAT_R32_SINT;
                byteWidth = 2 * 1;
                return;
            }
        }
    case D3D_REGISTER_COMPONENT_UINT32:
        {
            switch (varCount)
            {
            case 4:
                res = DXGI_FORMAT_R32G32B32A32_UINT;
                byteWidth = 4 * 4;
                return;
            case 3:
                res = DXGI_FORMAT_R32G32B32_UINT;
                byteWidth = 4 * 3;
                return;
            case 2:
                res = DXGI_FORMAT_R32G32_UINT;
                byteWidth = 4 * 2;
                return;
            case 1:
                res = DXGI_FORMAT_R32_UINT;
                byteWidth = 4 * 1;
            }
        }
    }
}


void Shader::BuildReflection(Renderer& device, const ComPtr<ID3D10Blob>& shaderBuffer)
{
    HRESULT result = 0;

    // create reflection interface 
    result = D3DReflect(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), IID_ID3D11ShaderReflection, static_cast<void**>(&m_pReflector));
    assert(FAILED(result));

    D3D11_SHADER_DESC desc;
    this->m_pReflector->GetDesc(&desc);

    // ----
    // fetch input schemantic descriptors
    this->m_inputElems.clear();

    LOGGER(Log::Logger().Trace("-- Reflection"));

    std::map<std::string, size_t> elemcount;

    std::vector<D3D11_INPUT_ELEMENT_DESC> elements;
    for (size_t i = 0; i < desc.InputParameters; i++)
    {
        D3D11_SIGNATURE_PARAMETER_DESC input_desc;
        this->m_pReflector->GetInputParameterDesc(i, &input_desc);

        // --- 
        // https://takinginitiative.wordpress.com/2011/12/11/directx-1011-basic-shader-reflection-automatic-input-layout-creation/

        InputElementRecord elem;
        ZeroMemory(&elem, sizeof(elem));

        // fill out input element desc
        D3D11_INPUT_ELEMENT_DESC elementDesc;
        elementDesc.SemanticName = input_desc.SemanticName;
        elementDesc.SemanticIndex = input_desc.SemanticIndex;
        elementDesc.InputSlot = 0;
        elementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
        elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        elementDesc.InstanceDataStepRate = 0;

        elem.desc = elementDesc;

        // count elements for numbering if an input array was specified
        elem.name = std::string(input_desc.SemanticName); // sor some reason implicit casting fails on some build (on VS15)

        auto elemit = elemcount.find(elem.name);
        if (elemit == elemcount.end()) { elemcount[elem.name] = 0; }
        else { elemit->second++; }

        // -- determine DXGI format
        this->GetDXGIFormat(input_desc, elementDesc.Format, elem.width);

        elements.push_back(elementDesc);
        this->m_inputElems.push_back(elem);
    }

    // add numbering for schemantic names
    size_t elementRelativeOffset = 0;
    // geezus.
    for (size_t i = 0; i < m_inputElems.size(); i++)
    {
        InputElementRecord& elem = m_inputElems[i];
        auto it = elemcount.find(elem.name);
        if (it == elemcount.end())
            continue;
        if (it->second > 0)
            elem.name = elem.name + std::to_string(elem.desc.SemanticIndex);

        LOGGER(Log::Logger().Trace("--- Input element: \"%s\" [%d], Format = {%d, %d}", elem.name.c_str(), elem.desc.SemanticIndex, elem.offset, elementRelativeOffset));
        elementRelativeOffset += elem.width;
    }

    // IL only needed for Vertex shader
    if (this->GetShaderType() == SHADER_VERTEX)
    {
        result = device->CreateInputLayout(&elements[0], elements.size(), shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), &this->m_layout);

        if (FAILED(result))
        THROW_EX_HRESULT(InputLayoutCreateException, result);
    }
    // ----
    // fetch output desctiptor
    for (uint32_t i = 0; i < desc.OutputParameters; i++)
    {
        D3D11_SIGNATURE_PARAMETER_DESC out_desc;
        this->m_pReflector->GetOutputParameterDesc(i, &out_desc);

        OutputTargetRecord rec;
        rec.desc = out_desc;

        LOGGER(Log::Logger().Trace("--- Output params: \"%s\" [%d]", out_desc.SemanticName, out_desc.SemanticIndex));

        m_outputTargets.push_back(rec);
    }

    // fetch constant buffers
    for (uint32_t i = 0; i < desc.ConstantBuffers; i++)
    {
        m_cBuffers.push_back({});

        CBRecord &cbRecord = m_cBuffers.front();
        cbRecord.m_slot = i;
        cbRecord.m_buffer = nullptr;

        ID3D11ShaderReflectionConstantBuffer* pConstBuffer = this->m_pReflector->GetConstantBufferByIndex(i);
        result = pConstBuffer->GetDesc(&cbRecord.m_description);
        if (FAILED(result))
        THROW_EX_HRESULT(ConstantBufferLocateException, result);

        // --- create buffer
        D3D11_BUFFER_DESC bufferDesc;
        bufferDesc.Usage = D3D11_USAGE_DYNAMIC; // D3D11_USAGE_DEFAULT;
        bufferDesc.ByteWidth = 16 * (ceil(cbRecord.m_description.Size / 16));
        bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        bufferDesc.MiscFlags = 0;
        bufferDesc.StructureByteStride = 0;

        result = device->CreateBuffer(&bufferDesc, nullptr, &cbRecord.m_buffer);
        if (FAILED(result))
        THROW_EX_HRESULT(ConstantBufferCreateException, result);

        cbRecord.m_cpuBuffer = std::unique_ptr<uint8_t>(new uint8_t[bufferDesc.ByteWidth]);
        ZeroMemory(cbRecord.m_cpuBuffer.get(), bufferDesc.ByteWidth);

        LOGGER(Log::Logger().Trace("--- Constant Buffer: \"%s\" [%d], Format = {t: %d, s: %d, bw: %d}",
            cbRecord.m_description.Name, i, cbRecord.m_description.Type, cbRecord.m_description.Size, bufferDesc.ByteWidth));

        // build up cbuffer variables

        for (uint32_t j = 0; j < cbRecord.m_description.Variables; j++)
        {
            ID3D11ShaderReflectionVariable* shader_variable = pConstBuffer->GetVariableByIndex(j);
            CBVar cbVar;

            // feltolt 
            result = shader_variable->GetDesc(&cbVar.m_var_desc);

            if (FAILED(result))
            THROW_EX_HRESULT(ConstantBufferLocateException, result);

            ID3D11ShaderReflectionType* sr_type = shader_variable->GetType();
            result = sr_type->GetDesc(&cbVar.m_type_desc);

            if (FAILED(result))
            THROW_EX_HRESULT(ConstantBufferLocateException, result);

            LOGGER(Log::Logger().Trace("---- Variable: \"%s\" [%d], Format = {t:%s, s: %d, o: %d}",
                cbVar.m_var_desc.Name, j, cbVar.m_type_desc.Name, cbVar.m_var_desc.Size, cbVar.m_var_desc.StartOffset));

            cbRecord.m_cbVars.push_back(cbVar);
            cbRecord.m_cbVarMap[cbVar.m_var_desc.Name] = j;
        }

        m_mapCBuffers[cbRecord.m_description.Name] = i;
    }

    // fetch bounded resources 

    for (size_t i = 0; i < desc.BoundResources; i++)
    {
        D3D11_SHADER_INPUT_BIND_DESC brDesc;
        result = this->m_pReflector->GetResourceBindingDesc(i, &brDesc);

        if (FAILED(result))
        THROW_EX_HRESULT(BoundResourceLocateException, result);

        BResRecord brRecord;

        brRecord.m_boundSource = nullptr;
        brRecord.m_desc = brDesc;

        LOGGER(Log::Logger().Trace("--- Bounded Resource: \"%s\" [%d], Format = {%d}",
            brRecord.m_desc.Name, i, brRecord.m_desc.Type));

        m_bResources.push_back(brRecord);
        m_mapBResources[brDesc.Name] = i;
    }

    // 
}


// =============================================================================================================================

HRESULT VertexShader::CompileShaderFromFile(LPCWCHAR file, const std::shared_ptr<D3D_SHADER_MACRO>& pDefines, const std::unique_ptr<ID3DInclude>& pInclude, LPCSTR entry, ComPtr<ID3D10Blob>& outShaderBuffer, ComPtr<ID3D10Blob>& outErrorMessage) { return D3DCompileFromFile(file, pDefines.get(), pInclude.get(), entry, VS_VERSION, D3D10_SHADER_ENABLE_STRICTNESS, 0, &outShaderBuffer, &outErrorMessage); }

HRESULT VertexShader::CompileShaderFromSource(LPCSTR source, size_t size, LPCSTR sourceName, const std::shared_ptr<D3D_SHADER_MACRO>& pDefines, const std::unique_ptr<ID3DInclude>& pInclude, LPCSTR entry, ComPtr<ID3D10Blob>& outShaderBuffer, ComPtr<ID3D10Blob>& outErrorMessage) { return D3DCompile(source, size, sourceName, pDefines.get(), pInclude.get(), entry, VS_VERSION, D3D10_SHADER_ENABLE_STRICTNESS | SHADER_COMPILE_FLAGS, 0, &outShaderBuffer, &outErrorMessage); }

void VertexShader::CreateShader(Renderer& render, ComPtr<ID3D10Blob> outShaderBuffer, const ComPtr<ID3D11ClassLinkage>& pClassLinkage)
{
    HRESULT result = render->CreateVertexShader(outShaderBuffer->GetBufferPointer(), outShaderBuffer->GetBufferSize(), pClassLinkage.Get(), &m_vxShader);
    if (FAILED(result))
    {
        THROW_EX_HRESULT(VSCrerateException, result);
    }
}

void VertexShader::SetSamplerPtr(Renderer& render, uint32_t bindPoint, uint32_t bindCount, const ComPtr<ID3D11SamplerState>& pSampler) { render.GetDeviceContext()->VSSetSamplers(bindPoint, bindCount, pSampler.GetAddressOf()); }

void VertexShader::SetShaderResources(Renderer& render, uint32_t bindPoint, uint32_t bindCount, const ComPtr<ID3D11ShaderResourceView>& pResView) { render.GetDeviceContext()->VSSetShaderResources(bindPoint, bindCount, pResView.GetAddressOf()); }


void VertexShader::SetConstantBuffer(Renderer& render, uint32_t slot, uint32_t numBuffers, const ComPtr<ID3D11Buffer>& buffer) { render.GetDeviceContext()->VSSetConstantBuffers(slot, numBuffers, buffer.GetAddressOf()); }

void VertexShader::BindShader(Renderer& render)
{
    render.GetDeviceContext()->IASetInputLayout(m_layout);
    render.GetDeviceContext()->VSSetShader(m_vxShader.Get(), nullptr, 0);
}

void VertexShader::UnbindShader(Renderer& render) { render.GetDeviceContext()->VSSetShader(nullptr, nullptr, 0); }

// =============================================================================================================================

HRESULT PixelShader::CompileShaderFromFile(LPCWCHAR file, const std::shared_ptr<D3D_SHADER_MACRO>& pDefines, const std::unique_ptr<ID3DInclude>& pInclude, LPCSTR entry, ComPtr<ID3D10Blob>& outShaderBuffer, ComPtr<ID3D10Blob>& outErrorMessage) { return D3DCompileFromFile(file, pDefines.get(), pInclude.get(), entry, PS_VERSION, D3D10_SHADER_ENABLE_STRICTNESS | SHADER_COMPILE_FLAGS, 0, &outShaderBuffer, &outErrorMessage); }

HRESULT PixelShader::CompileShaderFromSource(LPCSTR source, size_t size, LPCSTR sourceName, const std::shared_ptr<D3D_SHADER_MACRO>& pDefines, const std::unique_ptr<ID3DInclude>& pInclude, LPCSTR entry, ComPtr<ID3D10Blob>& outShaderBuffer, ComPtr<ID3D10Blob>& outErrorMessage) { return D3DCompile(source, size, sourceName, pDefines.get(), pInclude.get(), entry, PS_VERSION, D3D10_SHADER_ENABLE_STRICTNESS | SHADER_COMPILE_FLAGS, 0, &outShaderBuffer, &outErrorMessage); }

void PixelShader::CreateShader(Renderer& device, ComPtr<ID3D10Blob> outShaderBuffer, const ComPtr<ID3D11ClassLinkage>& pClassLinkage)
{
    HRESULT result = device->CreatePixelShader(outShaderBuffer->GetBufferPointer(), outShaderBuffer->GetBufferSize(), pClassLinkage.Get(), &m_pxShader);
    if (FAILED(result))
    {
        THROW_EX_HRESULT(VSCrerateException, result);
    }
}

void PixelShader::SetSamplerPtr(Renderer& render, uint32_t bindPoint, uint32_t bindCount, const ComPtr<ID3D11SamplerState>& pSampler) { render.GetDeviceContext()->PSSetSamplers(bindPoint, bindCount, pSampler.GetAddressOf()); }

void PixelShader::SetShaderResources(Renderer& render, uint32_t bindPoint, uint32_t bindCount, const ComPtr<ID3D11ShaderResourceView>& pResView) { render.GetDeviceContext()->PSSetShaderResources(bindPoint, bindCount, pResView.GetAddressOf()); }

void PixelShader::SetConstantBuffer(Renderer& render, uint32_t slot, uint32_t numBuffers, const ComPtr<ID3D11Buffer>& buffer) { render.GetDeviceContext()->PSSetConstantBuffers(slot, numBuffers, buffer.GetAddressOf()); }

void PixelShader::BindShader(Renderer& render) { render.GetDeviceContext()->PSSetShader(m_pxShader.Get(), nullptr, 0); }

void PixelShader::UnbindShader(Renderer& render) { render.GetDeviceContext()->PSSetShader(nullptr, nullptr, 0); }

// =============================================================================================================================
HRESULT GeometryShader::CompileShaderFromFile(LPCWCHAR file, const std::shared_ptr<D3D_SHADER_MACRO>& pDefines, const std::unique_ptr<ID3DInclude>& pInclude, LPCSTR entry, ComPtr<ID3D10Blob>& outShaderBuffer, ComPtr<ID3D10Blob>& outErrorMessage) { return D3DCompileFromFile(file, pDefines.get(), pInclude.get(), entry, GS_VERSION, D3D10_SHADER_ENABLE_STRICTNESS | SHADER_COMPILE_FLAGS, 0, outShaderBuffer.GetAddressOf(), outErrorMessage.GetAddressOf());}

HRESULT GeometryShader::CompileShaderFromSource(LPCSTR source, size_t size, LPCSTR sourceName, const std::shared_ptr<D3D_SHADER_MACRO>& pDefines, const std::unique_ptr<ID3DInclude>& pInclude, LPCSTR entry, ComPtr<ID3D10Blob>& outShaderBuffer, ComPtr<ID3D10Blob>& outErrorMessage) { return D3DCompile(source, size, sourceName, pDefines.get(), pInclude.get(), entry, GS_VERSION, D3D10_SHADER_ENABLE_STRICTNESS | SHADER_COMPILE_FLAGS, 0, outShaderBuffer.GetAddressOf(), outErrorMessage.GetAddressOf()); }

void GeometryShader::CreateShader(Renderer& render, ComPtr<ID3D10Blob> outShaderBuffer, const ComPtr<ID3D11ClassLinkage>& pClassLinkage) {
    HRESULT result = render->CreateGeometryShader(outShaderBuffer->GetBufferPointer(), outShaderBuffer->GetBufferSize(), pClassLinkage.Get(), &m_gmShader);
    if (FAILED(result))
    {
        THROW_EX_HRESULT(VSCrerateException, result);
    }
}

void GeometryShader::SetSamplerPtr(Renderer& render, uint32_t bindPoint, uint32_t bindCount, const ComPtr<ID3D11SamplerState>& pSampler) { render.GetDeviceContext()->GSSetSamplers(bindPoint, bindCount, pSampler.GetAddressOf()); }

void GeometryShader::SetConstantBuffer(Renderer& render, uint32_t slot, uint32_t numBuffers, const ComPtr<ID3D11Buffer>& buffer) { render.GetDeviceContext()->GSSetConstantBuffers(slot, numBuffers, buffer.GetAddressOf()); }

void GeometryShader::SetShaderResources(Renderer& render, uint32_t bindPoint, uint32_t bindCount, const ComPtr<ID3D11ShaderResourceView>& pResView) {render.GetDeviceContext()->GSSetShaderResources(bindPoint, bindCount, pResView.GetAddressOf());}

void GeometryShader::BindShader(Renderer& render) { render.GetDeviceContext()->GSSetShader(m_gmShader.Get(), nullptr, 0); }

void GeometryShader::UnbindShader(Renderer& render) { render.GetDeviceContext()->GSSetShader(nullptr, nullptr, 0); }
