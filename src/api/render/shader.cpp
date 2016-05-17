#include "../stdafx.h"

#include "shader.h"

#include <string>
#include <fstream>
#include <streambuf>
#include <iostream>

#include <cstdio>


using namespace std;
using namespace Grafkit;

using FWdebug::Exception;
using namespace FWdebugExceptions;

// =============================================================================================================================

Shader::Shader() : //IResource(),
	m_pxShader(nullptr),
	m_vxShader(nullptr),
	m_pReflector(nullptr),
	m_layout(nullptr),
	m_type(ST_NONE)
{
}


Shader::~Shader()
{
	this->Shutdown();
}

void Shader::LoadFromFile(Renderer & device, LPCSTR entry, LPCWCHAR file, ShaderType_e type)
{
	HRESULT result = 0;
	ID3D10Blob* errorMessage = nullptr;
	ID3D10Blob* shaderBuffer = nullptr;

	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements = 0;
	
	// D3D11_BUFFER_DESC matrixBufferDesc;

	// input checking
	if (!entry) throw EX(NullPointerException);
	if (!file) throw EX(NullPointerException);

	this->m_type = type;

	// Compile the vertex shader code.
	if (type == ST_Vertex) {
		result = D3DCompileFromFile(file, nullptr, nullptr, entry, "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &shaderBuffer, &errorMessage);
	}
	else if (type == ST_Pixel) {
		result = D3DCompileFromFile(file, nullptr, nullptr, entry, "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &shaderBuffer, &errorMessage);
	}

	if (FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if (errorMessage)
		{
			DispatchShaderErrorMessage(errorMessage, file, entry);
		}
		// If there was nothing in the error message then it simply could not find the shader file itself.
		else
		{
			throw EX(MissingShaderException);
		}
	}

	this->CompileShader(device, shaderBuffer);

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	shaderBuffer->Release();
}


void Shader::LoadFromMemory(Renderer & device, LPCSTR entry, LPCSTR source, size_t size,  ShaderType_e type)
{
	HRESULT result = 0;
	ID3D10Blob* errorMessage = nullptr;
	ID3D10Blob* shaderBuffer = nullptr;

	// input checking
	if (!entry) throw EX(NullPointerException);
	if (!source) throw EX(NullPointerException);

	this->m_type = type;

	// Compile the vertex shader code.
	if (type == ST_Vertex) {
		result = D3DCompile(source, size, nullptr, nullptr, nullptr, entry, "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &shaderBuffer, &errorMessage);
	}
	else if (type == ST_Pixel) {
		result = D3DCompile(source, size, nullptr, nullptr, nullptr, entry, "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &shaderBuffer, &errorMessage);
	}
	else {
		throw EX_DETAILS(MissingShaderException, "Attempting to load an unknown type of shader");
		return;
	}

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
	shaderBuffer->Release();
}


void Shader::Shutdown()
{
	if (this->m_pReflector){
		this->m_pReflector->Release();
		this->m_pReflector = nullptr;
	}

	if (this->m_layout) {
		this->m_layout->Release();
		this->m_layout = nullptr;
	}

	if (this->m_pxShader) {
		this->m_pxShader->Release();
		this->m_pxShader = nullptr;
	}
	if (this->m_vxShader) {
		this->m_vxShader->Release();
		this->m_vxShader = nullptr;
	}

	///@todo delete input layout elements
	///@todo delete constant buffer variables + buffer
	///@todo delete constant buffers
	///@todo delete bounded resources
}


void Shader::Render(ID3D11DeviceContext * deviceContext)
{
	m_pDC = deviceContext;
	
	/// @todo use switch-case as flow control
	// go the duck off 
	if (this->m_type == ST_Vertex) {
		// duck off the layout
		deviceContext->IASetInputLayout(m_layout);
		deviceContext->VSSetShader(m_vxShader, nullptr, 0);
	}
	else if (this->m_type == ST_Pixel) {
		deviceContext->PSSetShader(m_pxShader, nullptr, 0);
	}

	// duck the constant buffers around
	if (this->m_cBufferCount) 
	{

		for (size_t i = 0; i < this->m_cBufferCount; i++){
			ID3D11Buffer* buffer = this->m_cBuffers[i].m_buffer;
			UINT slot = this->m_cBuffers[i].m_slot;

			if (this->m_type == ST_Vertex) {
				deviceContext->VSSetConstantBuffers(slot, 1, &buffer);

			}
			else if (this->m_type == ST_Pixel) {
				deviceContext->PSSetConstantBuffers(slot, 1, &buffer);
			}
		}
	}

	// duck through the resources
	//if (this->m_bResourceCount) 
	{
		for (size_t i = 0; i < this->m_bResourceCount; i++) {
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

					if (m_type == ST_Vertex) {
						deviceContext->VSSetShaderResources(brRecord.m_desc.BindPoint, brRecord.m_desc.BindCount, &ppResV);
					}
					else if (m_type == ST_Pixel) {
						deviceContext->PSSetShaderResources(brRecord.m_desc.BindPoint, brRecord.m_desc.BindCount, &ppResV);
					}

				} break;

				case D3D_SIT_SAMPLER:
				{
					ID3D11SamplerState * pSampler = (ID3D11SamplerState*)brRecord.m_boundSource; // *(brRecord.m_boundSource);

					///@todo ezzel kell meg valamit kezdeni 
					if (m_type == ST_Vertex) {
						deviceContext->VSSetSamplers(brRecord.m_desc.BindPoint, brRecord.m_desc.BindCount, &pSampler);
					}
					else if (m_type == ST_Pixel) {
						deviceContext->PSSetSamplers(brRecord.m_desc.BindPoint, brRecord.m_desc.BindCount, &pSampler);
					}
				}break;
				
				}

				// zero, mindenesetre
				brRecord.m_boundSource = nullptr;
			}
		}
	}
}


void Shader::CompileShader(Renderer & device, ID3D10Blob* shaderBuffer)
{
	m_pDC = device.GetDeviceContext();

	LOGGER(Log::Logger().Info("--- Compiling shader"));

	HRESULT result = 0;
	if (this->m_type == ST_Vertex) {
		// Create the vertex shader from the buffer.
		result = device->CreateVertexShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), nullptr, &m_vxShader);
		if (FAILED(result))
		{
			throw EX(VSCrerateException);
		}
	}
	else if (this->m_type == ST_Pixel) {
		// Create the pixel shader from the buffer.
		result = device->CreatePixelShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), nullptr, &m_pxShader);
		if (FAILED(result))
		{
			throw EX(FSCrerateException);
		}
	}

	LOGGER(Log::Logger().Info("--- Compiling shader OK"));

	this->BuildReflection(device, shaderBuffer);
}

// ============================================================================================================================================

Shader::ShaderParamManager Shader::GetParam(std::string name)
{
	CBMap_it_t it = this->m_mapCBuffers.find(name);
	if (it != this->m_mapCBuffers.end()) {
		return GetParam(it->second);
	}
	return ShaderParamManager();
}

Shader::ShaderParamManager Shader::GetParam(size_t id)
{
	return (id < m_cBufferCount) ? ShaderParamManager(this, id) : ShaderParamManager();
}

Shader::ShaderParamManager Shader::GetParam(std::string name, std::string varname)
{
	CBMap_it_t it = this->m_mapCBuffers.find(name);
	if (it != this->m_mapCBuffers.end()) {
		return GetParam(it->second, varname);
	}
	
	return ShaderParamManager();
}

Shader::ShaderParamManager Shader::GetParam(size_t id, std::string varname)
{
	cb_variableMap_it it = m_cBuffers[id].m_cbVarMap.find(varname);
	if (it != this->m_cBuffers[id].m_cbVarMap.end()) {
		return GetParam(id, it->second);
	}

	return ShaderParamManager();
}

Shader::ShaderParamManager Shader::GetParam(size_t id, size_t vid)
{
	return (id < m_cBufferCount && vid < m_cBuffers[id].m_cbVarCount) ? ShaderParamManager(this, id, vid, 1) : ShaderParamManager();
}

// ============================================================================================================================================

void Shader::SetParamPtr(size_t id, const void const * pData, size_t size, size_t offset)
{
	if (id < m_cBufferCount) 
	{
		if (size == 0) {
			size = m_cBuffers[id].m_description.Size;
			offset = 0;
		}

		if (size + offset <= m_cBuffers[id].m_description.Size) 
		{
			CopyMemory(m_cBuffers[id].m_cpuBuffer + offset, pData, size);
		} else 
			DebugBreak();


		CopyMemory((BYTE*)this->MapParamBuffer(id), m_cBuffers[id].m_cpuBuffer, m_cBuffers[id].m_description.Size);
		this->UnMapParamBuffer(id);
	}

}

///@todo ez egyelore nem mukodik 
void Shader::SetParamPtr(size_t id, size_t vid, const void const * pData, size_t size, size_t offset)
{
	if (id < m_cBufferCount && vid < m_cBuffers[id].m_cbVarCount)
	{
		offset += m_cBuffers[id].m_cbVars[vid].m_var_desc.StartOffset;
		
		if (size == 0) {
			size = m_cBuffers[id].m_cbVars[vid].m_var_desc.Size;
		}

		if (size + offset <= m_cBuffers[id].m_cbVars[vid].m_var_desc.Size) 
		{
			CopyMemory(m_cBuffers[id].m_cpuBuffer + offset, pData, size);
		}
		else
			DebugBreak();

		CopyMemory((BYTE*)this->MapParamBuffer(id), m_cBuffers[id].m_cpuBuffer, m_cBuffers[id].m_description.Size);
		this->UnMapParamBuffer(id);
	}
}

void* Shader::MapParamBuffer(size_t id, int isDiscard)
{
	//if (id < m_cBufferCount) 
	{
		HRESULT result = 0;

		CBRecord & cbRecord = m_cBuffers[id];
		result = m_pDC->Map(cbRecord.m_buffer, 0, 	D3D11_MAP_WRITE_DISCARD, 0, &cbRecord.m_mappedResource);
		if (FAILED(result)) {
			throw EX_DETAILS(ConstantBufferMapException, "Cannot map buffer to a resource");
		}
		return cbRecord.m_mappedResource.pData;
	}  
	// else 	
	//	return nullptr;
}

void * Shader::GetMappedPtr(size_t id)
{
	// return (id < m_cBufferCount) ? m_cBuffers[id].m_mappedResource.pData : nullptr;
	return m_cBuffers[id].m_mappedResource.pData;
}

void Shader::UnMapParamBuffer(size_t id)
{
	//if (id < m_cBufferCount)
	{
		CBRecord & cbRecord = m_cBuffers[id];
		m_pDC->Unmap(cbRecord.m_buffer, 0);
	}
}


// ============================================================================================================================================

Shader::ShaderResourceManager Shader::GetBRes(std::string name)
{
	if (!this->m_mapBResources.empty()) {
		BResMap_it_t it = m_mapBResources.find(name);
		if (it != this->m_mapBResources.end()) {
			return GetBRes(it->second);
		}
	}
	return ShaderResourceManager();
}

Shader::ShaderResourceManager Shader::GetBRes(size_t id)
{
	return (id < m_bResourceCount) ? ShaderResourceManager(this, id) : ShaderResourceManager();
}

D3D11_SHADER_INPUT_BIND_DESC Shader::GetBResDesc(size_t id)
{
	return (id < m_bResourceCount) ? m_bResources[id].m_desc : D3D11_SHADER_INPUT_BIND_DESC();
}

void Shader::SetBResPointer(size_t id, void * ptr)
{
	if (id < m_bResourceCount)
		this->m_bResources[id].m_boundSource = ptr;
}

// ============================================================================================================================================

void Shader::DispatchShaderErrorMessage(ID3D10Blob* errorMessage, LPCWCHAR file, LPCSTR entry)
{
	char* compileErrors = nullptr;
	unsigned long bufferSize = 0;
	
	// duck this rainbow
	FILE* fp = nullptr;

	std::wstring error_string;

	compileErrors = (char*)(errorMessage->GetBufferPointer());
	bufferSize = errorMessage->GetBufferSize();

#if 0
	fopen_s(&fp, "shader-error.txt", "w");

	fputs(compileErrors, fp); fputs("\r\n", fp);

	fflush(fp);
	fclose(fp);
#endif 

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

		// -- determine DXGI format
		this->GetDXGIFormat(input_desc, elementDesc.Format, elem.width);

		// --- 

		elements.push_back(elementDesc);
		
		const char *name = elementDesc.SemanticName;
		
		this->m_mapInputElems.push_back(elem);
	}

	if (this->m_type == ST_Vertex) {
		result = device->CreateInputLayout(&elements[0], elements.size(), shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), &this->m_layout);

		if (FAILED(result)) {
			/// @todo throw exceptions 
			DebugBreak();
		}

	}


	// fetch output desctiptor
	for (size_t i = 0; i < desc.OutputParameters; i++)
	{
		D3D11_SIGNATURE_PARAMETER_DESC out_desc;
		this->m_pReflector->GetOutputParameterDesc(i, &out_desc);
		
		OutputTargetRecord rec;
		rec.desc = out_desc;
		
		m_outputTargets.push_back(rec);
	}

	// return; 

	// fetch constant buffers
	m_cBufferCount = desc.ConstantBuffers;
	m_cBuffers = new CBRecord[m_cBufferCount];

	for (size_t i = 0; i < desc.ConstantBuffers; i++)
	{
		CBRecord & cbRecord = m_cBuffers[i];
		cbRecord.m_slot = 0;
		cbRecord.m_buffer = nullptr;

		ID3D11ShaderReflectionConstantBuffer* pConstBuffer = this->m_pReflector->GetConstantBufferByIndex(i);
		result = pConstBuffer->GetDesc(&cbRecord.m_description);

		if (FAILED(result)) {
			throw EX_DETAILS(ConstantBufferLocateException, "Could not obtain description");
		}

		// --- create buffer
		// ezt lehet ki kellene venni innen, nem?
		D3D11_BUFFER_DESC bufferDesc;
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC; // D3D11_USAGE_DEFAULT;
		bufferDesc.ByteWidth = ((cbRecord.m_description.Size/16)+1)*16; 
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;

		result = device->CreateBuffer(&bufferDesc, nullptr, &cbRecord.m_buffer);
		if (FAILED(result)) {
			throw EX(ConstantBufferCreateException);
		}

		cbRecord.m_cpuBuffer = new BYTE[bufferDesc.ByteWidth];
		ZeroMemory(cbRecord.m_cpuBuffer, bufferDesc.ByteWidth);

		//LOGGER(LOG(TRACE) << "Constant buffer record" << cbRecord.m_description.Name << cbRecord.m_description.Size << cbRecord.m_description.Type);

		// build up cbuffer variables
		cbRecord.m_cbVarCount = cbRecord.m_description.Variables;
		cbRecord.m_cbVars = new CBVar[cbRecord.m_cbVarCount];

		for (size_t j = 0; j < cbRecord.m_cbVarCount; j++) {
			ID3D11ShaderReflectionVariable* shader_variable = pConstBuffer->GetVariableByIndex(j);
			CBVar &cbVar = cbRecord.m_cbVars[j];

			// feltolt 
			result = shader_variable->GetDesc(&cbVar.m_var_desc);

			if (FAILED(result)) {
				throw EX_DETAILS(ConstantBufferLocateException, "Could not obtain description for a cb variable");
			}

			ID3D11ShaderReflectionType* sr_type = shader_variable->GetType();
			result = sr_type->GetDesc(&cbVar.m_type_desc);

			if (FAILED(result)) {
				throw EX_DETAILS(ConstantBufferLocateException, "Could not obtain shader reflection type description");
			}

			// LOGGER(LOG(TRACE) << "Shader vairable created " << cbRecord.m_description.Name << cbVar.m_var_desc.Name << cbVar.m_type_desc.Name;);

			cbRecord.m_cbVarMap[cbVar.m_var_desc.Name] = j;
		}

		m_mapCBuffers[cbRecord.m_description.Name] = i;
	}

	// fetch bounded resources 
	this->m_bResourceCount = desc.BoundResources;
	this->m_bResources = new BResRecord[this->m_bResourceCount];

	for (size_t i = 0; i < m_bResourceCount; i++) {
		D3D11_SHADER_INPUT_BIND_DESC brDesc;
		result = this->m_pReflector->GetResourceBindingDesc(i, &brDesc);

		if (FAILED(result)) {
			throw EX(BoundResourceLocateException);
		}

		BResRecord &brRecord = this->m_bResources[i];

		brRecord.m_boundSource = nullptr;
		brRecord.m_desc = brDesc;

		m_mapBResources[brDesc.Name] = i;
	}

	// 
}

// =============================================================================================================================

void Shader::ShaderParamManager::SetF(const float v0)
{
	if (this->IsValid() && this->IsSubtype()) {
		float v[1];
		v[0] = v0;
		SetP(v);
	}
}

void Shader::ShaderParamManager::SetF(const float v0, const float v1)
{
	if (this->IsValid() && this->IsSubtype()) {
		float v[2];
		v[0] = v0;
		v[1] = v1;
		SetP(v);
	}
}

void Shader::ShaderParamManager::SetF(const float v0, const float v1, const float v2)
{
	if (this->IsValid() && this->IsSubtype()) {
		float v[3];
		v[0] = v0;
		v[1] = v1;
		v[2] = v2;
		SetP(v);
	}
}

void Shader::ShaderParamManager::SetF(const float v0, const float v1, const float v2, const float v3)
{
	if (this->IsValid() && this->IsSubtype()) {
		float v[4];
		v[0] = v0;
		v[1] = v1;
		v[2] = v2;
		v[3] = v3;
		SetP(v);
	}
}


// =============================================================================================================================
