#include <string>
#include <fstream>
#include <streambuf>
#include <iostream>

#include <cstdio>

#include "shader.h"
#include "../math/matrix.h"
#include "easyloggingpp.h"

using namespace std;
using namespace Grafkit;

using FWdebug::Exception;
using namespace FWdebugExceptions;

// =============================================================================================================================

ShaderParamManager::ShaderParamManager(Shader * shader, size_t id, size_t vid, int is_subtype)
	: m_pShader(shader),
	m_id(id), m_vid(vid), m_is_subtype(is_subtype)
{
}

Grafkit::ShaderParamManager::~ShaderParamManager()
{
}

ShaderParamManager ShaderParamManager::operator[](const char * name)
{
	if (this->IsValid() && this->IsSubtype()) {
		return m_pShader->GetParam(m_id, name);
	}
	return ShaderParamManager();
}

ShaderParamManager ShaderParamManager::operator[](size_t id)
{
	if (this->IsValid() && this->IsSubtype()) {
		return m_pShader->GetParam(m_id, id);
	}
	return ShaderParamManager();
}

size_t ShaderParamManager::GetVarCount()
{
	if (this->IsValid() && this->IsSubtype()) {
		return m_pShader->GetParamCount(m_id);
	}
	return 0;
}

// -----------------------------------------------------------------------------------------------------------------------------

void ShaderParamManager::Set(const void * const pData)
{
	return;
	if (this->IsValid()) {
		void* ptr = reinterpret_cast<void*>(((BYTE*)m_pShader->MapParamBuffer(m_id)));
		if (ptr) {
			memcpy(ptr, pData, m_pShader->GetCBDescription(m_id).Size);
		}
		m_pShader->UnMapParamBuffer(m_id);
	}
}

void ShaderParamManager::Set(const void * const pData, size_t offset, size_t width)
{
	return;
	if (this->IsValid()) {
		void* ptr = reinterpret_cast<void*>(((BYTE*)m_pShader->MapParamBuffer(m_id)) + offset);
		if (ptr) {
			memcpy(ptr, pData, width);
			m_pShader->UnMapParamBuffer(m_id);
		}
	}
}

void ShaderParamManager::Set(const float v0)
{
	return;
	if (this->IsValid() && this->IsSubtype()) {
		
		float* ptr = reinterpret_cast<float*>(((BYTE*)m_pShader->MapParamBuffer(m_id)) + m_pShader->GetCBVariableDescriptor(m_id, m_vid).StartOffset);
		if (ptr) {
			ptr[0] = v0;
		}
		m_pShader->UnMapParamBuffer(m_id);
	}
}

void ShaderParamManager::Set(const float v0, const float v1)
{
	return;
	if (this->IsValid() && this->IsSubtype()) {
		float* ptr = reinterpret_cast<float*>(((BYTE*)m_pShader->MapParamBuffer(m_id)) + m_pShader->GetCBVariableDescriptor(m_id, m_vid).StartOffset);
		if (ptr) {
			ptr[0] = v0; ptr[1] = v1;
		}
		m_pShader->UnMapParamBuffer(m_id);
	}
}

void ShaderParamManager::Set(const float v0, const float v1, const float v2)
{
	return;
	if (this->IsValid() && this->IsSubtype()) {
		float* ptr = reinterpret_cast<float*>(((BYTE*)m_pShader->MapParamBuffer(m_id)) + m_pShader->GetCBVariableDescriptor(m_id, m_vid).StartOffset);
		if (ptr) {
			ptr[0] = v0; ptr[1] = v1; ptr[2] = v2; 
		}
		m_pShader->UnMapParamBuffer(m_id);
	}
}

void ShaderParamManager::Set(const float v0, const float v1, const float v2, const float v3)
{
	return;
	if (this->IsValid() && this->IsSubtype()) {
		float* ptr = reinterpret_cast<float*>(((BYTE*)m_pShader->MapParamBuffer(m_id)) + m_pShader->GetCBVariableDescriptor(m_id, m_vid).StartOffset);
		if (ptr) {
			ptr[0] = v0; ptr[1] = v1; ptr[2] = v2; ptr[3] = v3;
		}
		m_pShader->UnMapParamBuffer(m_id);
	}
}

// -----------------------------------------------------------------------------------------------------------------------------

D3D11_MAPPED_SUBRESOURCE ShaderParamManager::GetappedResourceDesc()
{
	return D3D11_MAPPED_SUBRESOURCE();
}

D3D11_SHADER_BUFFER_DESC ShaderParamManager::GetBufferDesc()
{
	if (this->IsValid()) {

	}
	return D3D11_SHADER_BUFFER_DESC();
}

D3D11_SHADER_VARIABLE_DESC ShaderParamManager::GetVarDesc()
{
	if (this->IsValid() && this->IsSubtype()) {

	}
	return D3D11_SHADER_VARIABLE_DESC();
}

D3D11_SHADER_TYPE_DESC ShaderParamManager::GetTypeDesc()
{
	if (this->IsValid() && this->IsSubtype()) {

	}
	return D3D11_SHADER_TYPE_DESC();
}

// =============================================================================================================================

ShaderResourceManager::ShaderResourceManager(Shader * shader, size_t id) : 
m_pShader(shader), m_id(id)
{
}

void Grafkit::ShaderResourceManager::Set(void * ptr)
{
	if (this->IsValid()) {
		return m_pShader->SetBResPointer(m_id, ptr);
	}
}

inline D3D11_SHADER_INPUT_BIND_DESC Grafkit::ShaderResourceManager::GetBResDesc()
{
	if (this->IsValid()) {
		return m_pShader->GetBResDesc(m_id);
	}
	return D3D11_SHADER_INPUT_BIND_DESC();
}

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
		LOG(WARNING) << "Attempting to load an unknown type of shader" << type;
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
	if (!this->m_bResourceCount) {
		for (size_t i = 0; i < this->m_bResourceCount; i++) {
			BResRecord &brRecord = this->m_bResources[i];
			if (brRecord.m_boundSource != nullptr) {
				
				switch (brRecord.m_desc.Type) {

				case D3D_SIT_TEXTURE: 
				{
					///@todo ezzel kell meg valamit kezdeni 
					ID3D11ShaderResourceView * ppResV = (ID3D11ShaderResourceView*)brRecord.m_boundSource; // *(brRecord.m_boundSource);

					/// @todo a `brRecord.m_desc.BindCount`-al kezdj valamit plz
					if (brRecord.m_desc.BindCount != 1)
						DebugBreak();

					if (m_type == ST_Vertex) {
						deviceContext->VSSetShaderResources(brRecord.m_desc.BindPoint, brRecord.m_desc.BindCount, &ppResV);
					}
					else if (m_type == ST_Pixel) {
						deviceContext->PSSetShaderResources(brRecord.m_desc.BindPoint, brRecord.m_desc.BindCount, &ppResV);
					}

					// zero, mindenesetre
					brRecord.m_boundSource = nullptr;

				} break;

				case D3D_SIT_SAMPLER:
				{
					// ... 
				}break;
				
				}
			}
		}
	}
}


void Shader::CompileShader(Renderer & device, ID3D10Blob* shaderBuffer)
{
	m_pDC = device.GetDeviceContext();

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

	this->BuildReflection(device, shaderBuffer);
}

// ============================================================================================================================================

ShaderParamManager Shader::GetParam(std::string name)
{
	CBMap_it_t it = this->m_mapCBuffers.find(name);
	if (it != this->m_mapCBuffers.end()) {
		return GetParam(it->second);
	}
	return ShaderParamManager();
}

ShaderParamManager Shader::GetParam(size_t id)
{
	return ShaderParamManager(this, id);
}

ShaderParamManager Shader::GetParam(std::string name, std::string varname)
{
	CBMap_it_t it = this->m_mapCBuffers.find(name);
	if (it != this->m_mapCBuffers.end()) {
		return GetParam(it->second, varname);
	}
	
	return ShaderParamManager();
}

ShaderParamManager Shader::GetParam(size_t id, std::string varname)
{
	cb_variableMap_it it = m_cBuffers[id].m_cbVarMap.find(varname);
	if (it != this->m_cBuffers[id].m_cbVarMap.end()) {
		return GetParam(id, it->second);
	}

	return ShaderParamManager();
}

ShaderParamManager Shader::GetParam(size_t id, size_t vid)
{
	return ShaderParamManager(this, id, vid, 1);
}

// ============================================================================================================================================

void* Shader::MapParamBuffer(size_t id)
{
	if (id >= m_cBufferCount)
		return nullptr;

	return nullptr;
	
	HRESULT result = 0;

	CBRecord & cbRecord = m_cBuffers[id];
	 result = m_pDC->Map(cbRecord.m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &cbRecord.m_mappedResource);
	if (FAILED(result)) {
		throw EX_DETAILS(ConstantBufferMapException, "Cannot map buffer to a resource");
	}
	return cbRecord.m_mappedResource.pData;
}

void * Shader::GetMappedPtr(size_t id)
{
	if (id >= m_cBufferCount)
		return nullptr;
	
	return nullptr;
	
	return m_cBuffers[id].m_mappedResource.pData;
}

void Shader::UnMapParamBuffer(size_t id)
{
	if (id >= m_cBufferCount)
		return;

	CBRecord & cbRecord = m_cBuffers[id];
	m_pDC->Unmap(cbRecord.m_buffer, 0);
}

// ============================================================================================================================================

D3D11_MAPPED_SUBRESOURCE Shader::GetCBMappedResourceDesc(size_t id)
{
	return m_cBuffers[id].m_mappedResource;
}

D3D11_SHADER_BUFFER_DESC Shader::GetCBDescription(size_t id)
{
	return m_cBuffers[id].m_description;
}

D3D11_SHADER_VARIABLE_DESC Shader::GetCBVariableDescriptor(size_t id, size_t vid)
{
	return m_cBuffers[id].m_cbVars[vid].m_var_desc;
}

D3D11_SHADER_TYPE_DESC Shader::GetCBTypeDescriptor(size_t id, size_t vid)
{
	return m_cBuffers[id].m_cbVars[vid].m_type_desc;
}

// ============================================================================================================================================

ShaderParamManager Shader::GetBRes(std::string name)
{
	if (!this->m_mapBResources.empty()) {
		BResMap_it_t it = m_mapBResources.find(name);
		if (it != this->m_mapBResources.end()) {
			return GetBRes(it->second);
		}
	}
	return ShaderParamManager();
}

ShaderParamManager Shader::GetBRes(size_t id)
{
	return ShaderParamManager(this, id);
}

inline D3D11_SHADER_INPUT_BIND_DESC Grafkit::Shader::GetBResDesc(size_t id)
{
	return m_bResources[id].m_desc;
}

inline void Shader::SetBResPointer(size_t id, void * ptr)
{
	return; 
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

	LOG(ERROR) << compileErrors;

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

		// ... 
		///@todo fetch through rainbow 
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
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.ByteWidth = ((cbRecord.m_description.Size/16)+1)*16; 
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;

		result = device->CreateBuffer(&bufferDesc, nullptr, &cbRecord.m_buffer);
		if (FAILED(result)) {
			throw EX(ConstantBufferCreateException);
		}

		LOG(TRACE) << "Constant buffer record" << cbRecord.m_description.Name << cbRecord.m_description.Size << cbRecord.m_description.Type;

		// build up cbuffer variables
		cbRecord.m_cbVarCount = cbRecord.m_description.Variables;
		cbRecord.m_cbVars = new CBVar[cbRecord.m_cbVarCount];

		for (size_t j = 0; j < cbRecord.m_cbVarCount; j++) {
			ID3D11ShaderReflectionVariable* shader_variable = pConstBuffer->GetVariableByIndex(i);
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

			LOG(TRACE) << "Shader vairable created " << cbRecord.m_description.Name << cbVar.m_var_desc.Name << cbVar.m_type_desc.Name;

			cbRecord.m_cbVarMap[cbVar.m_var_desc.Name] = j;
		}

		this->m_mapCBuffers[cbRecord.m_description.Name] = i;
	}

	return; 

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

// ============================================================================================================
