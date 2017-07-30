#define INITGUID 

#include "stdafx.h"

#include <string>
#include <fstream>
#include <streambuf>
#include <iostream>

#include <cstdio>

#include "shader.h"

using namespace std;
using namespace Grafkit;

using FWdebug::Exception;
using namespace FWdebugExceptions;

#define PS_VERSION "ps_4_0"
#define VS_VERSION "vs_4_0"
#define GS_VERSION "gs_4_0"


// TODO http://stackoverflow.com/questions/24323281/the-pixel-shader-unit-expects-a-sampler

// =============================================================================================================================

Shader::Shader() :
	m_pReflector(nullptr),
	m_layout(nullptr)
{
}


Shader::~Shader()
{
	this->Shutdown();
}


void Shader::LoadFromFile(Renderer & device, LPCSTR entry, LPCWCHAR file, ID3DInclude* pInclude, D3D_SHADER_MACRO* pDefines)
{
	HRESULT result = 0;
	ID3D10Blob* errorMessage = nullptr;
	ID3D10Blob* shaderBuffer = nullptr;
	unsigned int numElements = 0;
	
	// input checking
	if (!entry) throw EX(NullPointerException);	if (!file) throw EX(NullPointerException);

	result = CompileShaderFromFile(file, pDefines, pInclude, entry, shaderBuffer, errorMessage);

	if (FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if (errorMessage)
			DispatchShaderErrorMessage(errorMessage, file, entry);
		// If there was nothing in the error message then it simply could not find the shader file itself.
		else
			throw EX_HRESULT(MissingShaderException, result);
	}

	this->CompileShader(device, shaderBuffer);

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	RELEASE(shaderBuffer);
	RELEASE(errorMessage);
}


void Shader::LoadFromMemory(Renderer & device, LPCSTR entry, LPCSTR source, size_t size, LPCSTR name, ID3DInclude* pInclude, D3D_SHADER_MACRO* pDefines)
{
	HRESULT result = 0;
	ID3D10Blob* errorMessage = nullptr;
	ID3D10Blob* shaderBuffer = nullptr;

	// input checking
	if (!entry) throw EX(NullPointerException);
	if (!source) throw EX(NullPointerException);

	result = CompileShaderFromSource(source, size, name, pDefines, pInclude, entry, shaderBuffer, errorMessage);

	if (FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if (errorMessage)
		{
			DispatchShaderErrorMessage(errorMessage, nullptr, entry);
		}
		// If there was nothing in the error message then it simply could not find the shader file itself.
		else
		{
			throw EX(MissingShaderException);
		}
	}

	this->CompileShader(device, shaderBuffer);

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	RELEASE(shaderBuffer);
	RELEASE(errorMessage);
}


void Shader::Shutdown()
{
	LOGGER(Log::Logger().Info("Shader destructor"));

	// this->ShutdownChild();

	// RELEASE(this->m_pReflector);
	// RELEASE(this->m_layout);
	/*
	for (size_t i = 0; i < GetParamCount(); i++) {
		RELEASE(m_cBuffers[i].m_buffer);
		delete[] m_cBuffers[i].m_cpuBuffer; m_cBuffers[i].m_cpuBuffer = nullptr;
	}
	*/

	///@todo delete input layout elements
	///@todo delete constant buffer variables + buffer
	///@todo delete constant buffers
	///@todo delete bounded resources
}


void Shader::Bind(ID3D11DeviceContext * deviceContext)
{
	BindShader(deviceContext);

	// duck the constant buffers around
	if (this->GetParamCount()) 
	{
		for (size_t i = 0; i < this->GetParamCount(); i++){
			ID3D11Buffer* buffer = this->m_cBuffers[i].m_buffer;
			UINT slot = this->m_cBuffers[i].m_slot;

			SetConstantBuffer(deviceContext, slot, 1, buffer);
		}
	}

	// duck through the resources
	if (this->GetBoundedResourceCount()) 
	{
		for (size_t i = 0; i < this->GetBoundedResourceCount(); i++) {
			BResRecord &brRecord = this->m_bResources[i];
			if (brRecord.m_boundSource != nullptr) {
				
				/// @todo a `brRecord.m_desc.BindCount`-al kezdj valamit plz
				if (brRecord.m_desc.BindCount != 1)
					DebugBreak();

				switch (brRecord.m_desc.Type) {
				case D3D_SIT_TEXTURE: 
				{
					///@todo ezzel kell meg valamit kezdeni 
					ID3D11ShaderResourceView * ppResV = (ID3D11ShaderResourceView*)brRecord.m_boundSource; // *(brRecord.m_boundSource);

					SetShaderResources(deviceContext, brRecord.m_desc.BindPoint, brRecord.m_desc.BindCount, ppResV);

				} break;

				case D3D_SIT_SAMPLER:
				{
					ID3D11SamplerState * pSampler = (ID3D11SamplerState*)brRecord.m_boundSource; // *(brRecord.m_boundSource);
					SetSamplerPtr(deviceContext, brRecord.m_desc.BindPoint, brRecord.m_desc.BindCount, pSampler);
				}break;
				
				}

				// zero, mindenesetre
				// brRecord.m_boundSource = nullptr;
			}
		}
	}
}

void Grafkit::Shader::Unbind(ID3D11DeviceContext * deviceContext)
{
	UnbindShader(deviceContext);
}

void Shader::CompileShader(Renderer & device, ID3D10Blob* shaderBuffer)
{
	LOGGER(Log::Logger().Info("- Compiling shader"));
	CreateShader(device.GetDevice(), shaderBuffer);
	LOGGER(Log::Logger().Info("- Compiling shader OK"));

	this->BuildReflection(device, shaderBuffer);
}

// ============================================================================================================================================
// set param
void Grafkit::Shader::SetParam(ID3D11DeviceContext * deviceContext, std::string name, const void * const pData, size_t size, size_t offset)
{
	auto it = this->m_mapCBuffers.find(name);
	if (it != m_mapCBuffers.end()) {
		SetParam(deviceContext, it->second, pData, size, offset);
	}
}

void Grafkit::Shader::SetParam(ID3D11DeviceContext * deviceContext, size_t id, const void * const pData, size_t size, size_t offset)
{
	if (id < GetParamCount())
	{
		if (size == 0) {
			size = m_cBuffers[id].m_description.Size;
			offset = 0;
		}

		if (size + offset <= m_cBuffers[id].m_description.Size)
			CopyMemory(m_cBuffers[id].m_cpuBuffer + offset, pData, size);
		else
			DebugBreak();

		CopyMemory((BYTE*)this->MapParamBuffer(deviceContext, id), m_cBuffers[id].m_cpuBuffer, m_cBuffers[id].m_description.Size);
		this->UnMapParamBuffer(deviceContext, id);
	}
	else {
		DebugBreak();
	}
}

// set value inside a value 
void Grafkit::Shader::SetParamValue(ID3D11DeviceContext * deviceContext, std::string name, std::string valueName, void * const pData, size_t size, size_t offset)
{
	auto it = this->m_mapCBuffers.find(name);
	if (it != m_mapCBuffers.end()) {
		auto vit = this->m_cBuffers[it->second].m_cbVarMap.find(valueName);
		SetParamValue(deviceContext, it->second, vit->second, pData, size, offset);
	}
}

void Grafkit::Shader::SetParamValue(ID3D11DeviceContext * deviceContext, size_t id, size_t valueid, const void * const pData, size_t size, size_t offset)
{
	if (id < GetParamCount() && valueid < GetParamValueCount(id))
	{
		offset += m_cBuffers[id].m_cbVars[valueid].m_var_desc.StartOffset;

		if (size == 0)
			size = m_cBuffers[id].m_cbVars[valueid].m_var_desc.Size;

		if (size + offset <= m_cBuffers[id].m_description.Size)
			CopyMemory(m_cBuffers[id].m_cpuBuffer + offset, pData, size);
		else
			DebugBreak();

		BYTE* p = (BYTE*)this->MapParamBuffer(deviceContext, id);
		CopyMemory(p, m_cBuffers[id].m_cpuBuffer, m_cBuffers[id].m_description.Size);
		this->UnMapParamBuffer(deviceContext, id);
	}
	else {
		DebugBreak();
	}
}

void* Shader::MapParamBuffer(ID3D11DeviceContext * deviceContext, size_t id, int isDiscard)
{
	HRESULT result = 0;

	CBRecord & cbRecord = m_cBuffers[id];
	result = deviceContext->Map(cbRecord.m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &cbRecord.m_mappedResource);
	if (FAILED(result))
		throw EX_HRESULT(ConstantBufferMapException, result);

	return cbRecord.m_mappedResource.pData; 
}

void * Shader::GetMappedPtr(size_t id)
{
	return m_cBuffers[id].m_mappedResource.pData;
}

void Shader::UnMapParamBuffer(ID3D11DeviceContext * deviceContext, size_t id)
{
	//if (id < m_cBufferCount)
	{
		CBRecord & cbRecord = m_cBuffers[id];
		deviceContext->Unmap(cbRecord.m_buffer, 0);
	}
}


// ============================================================================================================================================

void Grafkit::Shader::SetBoundedResourcePointer(std::string name, void * ptr)
{
	auto it = this->m_mapBResources.find(name);
	if (it != this->m_mapBResources.end())
		SetBoundedResourcePointer(it->second, ptr);
}

void Grafkit::Shader::SetBoundedResourcePointer(size_t id, void * ptr)
{
	if (id < GetBoundedResourceCount())
		this->m_bResources[id].m_boundSource = ptr;
}

void Shader::DispatchShaderErrorMessage(ID3D10Blob* errorMessage, LPCWCHAR file, LPCSTR entry)
{
	char* compileErrors = nullptr;
	unsigned long bufferSize = 0;
	
	// duck this rainbow
	FILE* fp = nullptr;

	std::wstring error_string;

	compileErrors = (char*)(errorMessage->GetBufferPointer());
	bufferSize = errorMessage->GetBufferSize();

	// LOGGER(LOG(ERROR) << compileErrors);
	LOGGER(Log::Logger().Error(compileErrors));

	errorMessage->Release();
	errorMessage = 0;

	// @todo add compile errors text 
	throw EX_DETAILS(ShaderException, "See logfiles");
}


void Shader::GetDXGIFormat(D3D11_SIGNATURE_PARAMETER_DESC pd, DXGI_FORMAT &res, UINT &byteWidth) {
	BYTE mask = pd.Mask;
	int varCount = 0;
	while (mask)
	{
		if (mask & 0x01) varCount++;
		mask >>= 1; 
	}

	res = DXGI_FORMAT_UNKNOWN;
	byteWidth = 0;

	///@todo I should took this mess into a LUT
	switch (pd.ComponentType) {
		case D3D_REGISTER_COMPONENT_FLOAT32:
		{
			switch (varCount) {
			case 4:
				res = DXGI_FORMAT_R32G32B32A32_FLOAT;
				byteWidth = 4*4;
				return;
			case 3:
				res = DXGI_FORMAT_R32G32B32_FLOAT;
				byteWidth = 4*3;
				return;
			case 2:
				res = DXGI_FORMAT_R32G32_FLOAT;
				byteWidth = 4*2;
				return;
			case 1:
				res = DXGI_FORMAT_R32_FLOAT;
				byteWidth = 4*1;
				return;
			}
			break;
		}
		case D3D_REGISTER_COMPONENT_SINT32:
		{
			switch (varCount) {
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
			switch (varCount) {
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
				return;
			}
		}
	}
}


void Shader::BuildReflection(Renderer & device, ID3D10Blob* shaderBuffer)
{
	HRESULT result = 0;

	// create reflection interface 
	result = D3DReflect(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&m_pReflector);
	if (FAILED(result)) {
		/// @todo throw exception
		DebugBreak();
	}

	D3D11_SHADER_DESC desc;
	this->m_pReflector->GetDesc(&desc);

	// fetch input descriptors
	this->m_mapInputElems.clear();

	LOGGER(Log::Logger().Trace("-- Reflection"));

	std::vector<D3D11_INPUT_ELEMENT_DESC> elements;
	for (UINT i = 0; i < desc.InputParameters; i++)
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

		// -- determine DXGI format
		this->GetDXGIFormat(input_desc, elementDesc.Format, elem.width);

		LOGGER(Log::Logger().Trace("--- Input element: \"%s\" [%d], Format = {%d, %d}", input_desc.SemanticName, input_desc.SemanticIndex, elementDesc.Format, elem.width));

		elements.push_back(elementDesc);
		
		const char *name = elementDesc.SemanticName;
		
		this->m_mapInputElems.push_back(elem);
	}

	// IL csak V-shaderhez kell
	if (this->GetShaderType() == SHADER_VERTEX) {
		result = device->CreateInputLayout(&elements[0], elements.size(), shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), &this->m_layout);

		if (FAILED(result))
			throw new EX_HRESULT(InputLayoutCreateException, result);
	}

	// fetch output desctiptor
	for (UINT i = 0; i < desc.OutputParameters; i++)
	{
		D3D11_SIGNATURE_PARAMETER_DESC out_desc;
		this->m_pReflector->GetOutputParameterDesc(i, &out_desc);
		
		OutputTargetRecord rec;
		rec.desc = out_desc;
		
		LOGGER(Log::Logger().Trace("--- Output params: \"%s\" [%d]", out_desc.SemanticName, out_desc.SemanticIndex));

		m_outputTargets.push_back(rec);
	}

	// fetch constant buffers
	for (UINT i = 0; i < desc.ConstantBuffers; i++)
	{
		CBRecord cbRecord; // = m_cBuffers[i];
		cbRecord.m_slot = i;
		cbRecord.m_buffer = nullptr;

		ID3D11ShaderReflectionConstantBuffer* pConstBuffer = this->m_pReflector->GetConstantBufferByIndex(i);
		result = pConstBuffer->GetDesc(&cbRecord.m_description);

		if (FAILED(result))
			throw EX_HRESULT(ConstantBufferLocateException, result);

		// --- create buffer
		D3D11_BUFFER_DESC bufferDesc;
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC; // D3D11_USAGE_DEFAULT;
		bufferDesc.ByteWidth = 16 * (ceil(cbRecord.m_description.Size/16));
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;

		result = device->CreateBuffer(&bufferDesc, nullptr, &cbRecord.m_buffer);
		if (FAILED(result))
			throw EX_HRESULT(ConstantBufferCreateException, result);

		cbRecord.m_cpuBuffer = new BYTE[bufferDesc.ByteWidth];
		ZeroMemory(cbRecord.m_cpuBuffer, bufferDesc.ByteWidth);

		LOGGER(Log::Logger().Trace("--- Constant Buffer: \"%s\" [%d], Format = {t: %d, s: %d, bw: %d}", 
			cbRecord.m_description.Name, i, cbRecord.m_description.Type, cbRecord.m_description.Size, bufferDesc.ByteWidth));

		// build up cbuffer variables

		for (UINT j = 0; j < cbRecord.m_description.Variables; j++) {
			ID3D11ShaderReflectionVariable* shader_variable = pConstBuffer->GetVariableByIndex(j);
			CBVar cbVar;

			// feltolt 
			result = shader_variable->GetDesc(&cbVar.m_var_desc);

			if (FAILED(result))
				throw EX_HRESULT(ConstantBufferLocateException, result);

			ID3D11ShaderReflectionType* sr_type = shader_variable->GetType();
			result = sr_type->GetDesc(&cbVar.m_type_desc);

			if (FAILED(result))
				throw EX_HRESULT(ConstantBufferLocateException, result);

			LOGGER(Log::Logger().Trace("---- Variable: \"%s\" [%d], Format = {t:%s, s: %d, o: %d}", 
				cbVar.m_var_desc.Name, j, cbVar.m_type_desc.Name, cbVar.m_var_desc.Size, cbVar.m_var_desc.StartOffset));

			cbRecord.m_cbVars.push_back(cbVar);
			cbRecord.m_cbVarMap[cbVar.m_var_desc.Name] = j;
		}

		m_cBuffers.push_back(cbRecord);
		m_mapCBuffers[cbRecord.m_description.Name] = i;
	}

	// fetch bounded resources 

	for (size_t i = 0; i < desc.BoundResources; i++) {
		D3D11_SHADER_INPUT_BIND_DESC brDesc;
		result = this->m_pReflector->GetResourceBindingDesc(i, &brDesc);

		if (FAILED(result))
			throw EX_HRESULT(BoundResourceLocateException, result);

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

Grafkit::VertexShader::VertexShader() : Shader(),
m_vxShader(nullptr)
{
}


Grafkit::VertexShader::~VertexShader()
{
}


void Grafkit::VertexShader::ShutdownChild()
{
	RELEASE(m_vxShader);
}


HRESULT Grafkit::VertexShader::CompileShaderFromFile(LPCWCHAR file, D3D_SHADER_MACRO * pDefines, ID3DInclude * pInclude, LPCSTR entry, ID3D10Blob *& shaderBuffer, ID3D10Blob *& errorMessage)
{
	return D3DCompileFromFile(file, pDefines, pInclude, entry, VS_VERSION, D3D10_SHADER_ENABLE_STRICTNESS, 0, &shaderBuffer, &errorMessage);
}


HRESULT Grafkit::VertexShader::CompileShaderFromSource(LPCSTR source, size_t size, LPCSTR sourceName, D3D_SHADER_MACRO * pDefines, ID3DInclude * pInclude, LPCSTR entry, ID3D10Blob *& shaderBuffer, ID3D10Blob *& errorMessage)
{
	return  D3DCompile(source, size, sourceName, pDefines, pInclude, entry, VS_VERSION, D3D10_SHADER_ENABLE_STRICTNESS, 0, &shaderBuffer, &errorMessage);
}


void Grafkit::VertexShader::CreateShader(ID3D11Device* device, ID3D10Blob* shaderBuffer, ID3D11ClassLinkage * pClassLinkage)
{
	HRESULT result = device->CreateVertexShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), pClassLinkage, &m_vxShader);
	if (FAILED(result))
		throw EX_HRESULT(VSCrerateException, result);
}


void Grafkit::VertexShader::SetSamplerPtr(ID3D11DeviceContext* device, UINT bindPoint, UINT bindCount, ID3D11SamplerState *& pSampler)
{
	device->VSSetSamplers(bindPoint, bindCount, &pSampler);
}


void Grafkit::VertexShader::SetShaderResources(ID3D11DeviceContext * device, UINT bindPoint, UINT bindCount, ID3D11ShaderResourceView *& pResView)
{
	device->VSSetShaderResources(bindPoint, bindCount, &pResView);
}


void Grafkit::VertexShader::SetConstantBuffer(ID3D11DeviceContext * device, UINT slot, UINT numBuffers, ID3D11Buffer *& buffer)
{
	device->VSSetConstantBuffers(slot, numBuffers, &buffer); 
}


void Grafkit::VertexShader::BindShader(ID3D11DeviceContext * device)
{
	device->IASetInputLayout(m_layout);
	device->VSSetShader(m_vxShader, nullptr, 0);
}

void Grafkit::VertexShader::UnbindShader(ID3D11DeviceContext * device)
{
	device->VSSetShader(nullptr, nullptr, 0);
}

// =============================================================================================================================

Grafkit::PixelShader::PixelShader() : Shader(),
m_pxShader(nullptr)
{
}

Grafkit::PixelShader::~PixelShader()
{
}


void Grafkit::PixelShader::ShutdownChild()
{
	RELEASE(m_pxShader);
}


HRESULT Grafkit::PixelShader::CompileShaderFromFile(LPCWCHAR file, D3D_SHADER_MACRO * pDefines, ID3DInclude * pInclude, LPCSTR entry, ID3D10Blob *& shaderBuffer, ID3D10Blob *& errorMessage)
{
	return D3DCompileFromFile(file, pDefines, pInclude, entry, PS_VERSION, D3D10_SHADER_ENABLE_STRICTNESS, 0, &shaderBuffer, &errorMessage);
}


HRESULT Grafkit::PixelShader::CompileShaderFromSource(LPCSTR source, size_t size, LPCSTR sourceName, D3D_SHADER_MACRO * pDefines, ID3DInclude * pInclude, LPCSTR entry, ID3D10Blob *& shaderBuffer, ID3D10Blob *& errorMessage)
{
	return  D3DCompile(source, size, sourceName, pDefines, pInclude, entry, PS_VERSION, D3D10_SHADER_ENABLE_STRICTNESS, 0, &shaderBuffer, &errorMessage);
}


void Grafkit::PixelShader::CreateShader(ID3D11Device * device, ID3D10Blob * shaderBuffer, ID3D11ClassLinkage * pClassLinkage)
{
	HRESULT result = device->CreatePixelShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), pClassLinkage, &m_pxShader);
	if (FAILED(result))
		throw EX_HRESULT(PSCrerateException, result);
}


void Grafkit::PixelShader::SetSamplerPtr(ID3D11DeviceContext * device, UINT bindPoint, UINT bindCount, ID3D11SamplerState *& pSampler)
{
	device->PSSetSamplers(bindPoint, bindCount, &pSampler);
}

void Grafkit::PixelShader::SetShaderResources(ID3D11DeviceContext * device, UINT bindPoint, UINT bindCount, ID3D11ShaderResourceView *& pResView)
{
	device->PSSetShaderResources(bindPoint, bindCount, &pResView);
}


void Grafkit::PixelShader::SetConstantBuffer(ID3D11DeviceContext * device, UINT slot, UINT numBuffers, ID3D11Buffer *& buffer)
{
	device->PSSetConstantBuffers(slot, numBuffers, &buffer);
}


void Grafkit::PixelShader::BindShader(ID3D11DeviceContext * device)
{
	device->PSSetShader(m_pxShader, nullptr, 0);
}

void Grafkit::PixelShader::UnbindShader(ID3D11DeviceContext * device)
{
	device->PSSetShader(nullptr, nullptr, 0);
}

// =============================================================================================================================

Grafkit::GeometryShader::GeometryShader() :
	m_gmShader(NULL)
{
}

Grafkit::GeometryShader::~GeometryShader()
{
}

void Grafkit::GeometryShader::ShutdownChild()
{
	RELEASE(m_gmShader);
}

HRESULT Grafkit::GeometryShader::CompileShaderFromFile(LPCWCHAR file, D3D_SHADER_MACRO * pDefines, ID3DInclude * pInclude, LPCSTR entry, ID3D10Blob *& shaderBuffer, ID3D10Blob *& errorMessage)
{
	return D3DCompileFromFile(file, pDefines, pInclude, entry, GS_VERSION, D3D10_SHADER_ENABLE_STRICTNESS, 0, &shaderBuffer, &errorMessage);
}

HRESULT Grafkit::GeometryShader::CompileShaderFromSource(LPCSTR source, size_t size, LPCSTR sourceName, D3D_SHADER_MACRO * pDefines, ID3DInclude * pInclude, LPCSTR entry, ID3D10Blob *& shaderBuffer, ID3D10Blob *& errorMessage)
{
	return  D3DCompile(source, size, sourceName, pDefines, pInclude, entry, GS_VERSION, D3D10_SHADER_ENABLE_STRICTNESS, 0, &shaderBuffer, &errorMessage);
}

void Grafkit::GeometryShader::CreateShader(ID3D11Device * device, ID3D10Blob * shaderBuffer, ID3D11ClassLinkage * pClassLinkage)
{
	HRESULT result = device->CreateGeometryShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), pClassLinkage, &m_gmShader);
	if (FAILED(result))
		throw EX_HRESULT(PSCrerateException, result);
}

void Grafkit::GeometryShader::SetSamplerPtr(ID3D11DeviceContext * device, UINT bindPoint, UINT bindCount, ID3D11SamplerState *& pSampler)
{
	device->PSSetSamplers(bindPoint, bindCount, &pSampler);
}

void Grafkit::GeometryShader::SetShaderResources(ID3D11DeviceContext * device, UINT bindPoint, UINT bindCount, ID3D11ShaderResourceView *& pResView)
{
	device->PSSetShaderResources(bindPoint, bindCount, &pResView);
}

void Grafkit::GeometryShader::SetConstantBuffer(ID3D11DeviceContext * device, UINT slot, UINT numBuffers, ID3D11Buffer *& buffer)
{
	device->GSSetConstantBuffers(slot, numBuffers, &buffer);
}

void Grafkit::GeometryShader::BindShader(ID3D11DeviceContext * device)
{
	device->GSSetShader(m_gmShader, nullptr, 0);
}

void Grafkit::GeometryShader::UnbindShader(ID3D11DeviceContext * device)
{
	device->GSSetShader(nullptr, nullptr, 0);
}
