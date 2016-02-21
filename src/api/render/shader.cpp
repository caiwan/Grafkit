#include <string>
#include <fstream>
#include <streambuf>
#include <iostream>

#include <cstdio>

#include "shader.h"
#include "../math/matrix.h"
#include "easyloggingpp.h"

using namespace std;
using namespace FWrender;

using FWrender::Shader;

using namespace FWdebugExceptions;

using FWmath::Matrix;

// =============================================================================================================================

Shader::Shader() : IRenderAsset(),
	m_pxShader(NULL),
	m_vxShader(NULL),
	m_pReflector(NULL),
	m_layout(NULL),
	m_type(ST_NONE)
{
}


Shader::~Shader()
{
	this->Shutdown();
}

void Shader::LoadFromFile(ID3D11Device * const & device, LPCSTR entry, LPCWCHAR file, ShaderType_e type)
{
	HRESULT result = 0;
	ID3D10Blob* errorMessage = NULL;
	ID3D10Blob* shaderBuffer = NULL;

	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements = 0;
	
	// D3D11_BUFFER_DESC matrixBufferDesc;

	// input checking
	if (!entry) throw new EX(NullPointerException);
	if (!file) throw new EX(NullPointerException);

	this->m_type = type;

	// Compile the vertex shader code.
	if (type == ST_Vertex) {
		result = D3DCompileFromFile(file, NULL, NULL, entry, "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &shaderBuffer, &errorMessage);
	}
	else if (type == ST_Pixel) {
		result = D3DCompileFromFile(file, NULL, NULL, entry, "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &shaderBuffer, &errorMessage);
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
			throw new EX(MissingShaderException);
		}
	}

	this->CompileShader(device, shaderBuffer);

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	shaderBuffer->Release();
}


void Shader::LoadFromMemory(ID3D11Device * const & device, LPCSTR entry, LPCSTR source, size_t size,  ShaderType_e type)
{
	HRESULT result = 0;
	ID3D10Blob* errorMessage = NULL;
	ID3D10Blob* shaderBuffer = NULL;

	// D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements = 0;

	// D3D11_BUFFER_DESC matrixBufferDesc;

	// input checking
	if (!entry) throw EX(NullPointerException);
	if (!source) throw EX(NullPointerException);

	this->m_type = type;

	// Compile the vertex shader code.
	if (type == ST_Vertex) {
		result = D3DCompile(source, size, NULL, NULL, NULL, entry, "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &shaderBuffer, &errorMessage);
	}
	else if (type == ST_Pixel) {
		result = D3DCompile(source, size, NULL, NULL, NULL, entry, "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &shaderBuffer, &errorMessage);
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
	if (this->m_pReflector)
		this->m_pReflector->Release();

	if (this->m_pxShader) this->m_pxShader->Release();
	if (this->m_vxShader) this->m_vxShader->Release();

	this->m_inputNames.clear();
}


void Shader::Render(ID3D11DeviceContext * deviceContext)
{
	/// @todo use switch-case as flow control
	// go the duck off 
	if (this->m_type == ST_Vertex) {
		// duck off the layout
		deviceContext->IASetInputLayout(m_layout);
		deviceContext->VSSetShader(m_vxShader, NULL, 0);
	}
	else if (this->m_type == ST_Pixel) {
		deviceContext->PSSetShader(m_pxShader, NULL, 0);
	}

	// duck the constant buffers around
	if (!this->m_vBuffers.empty()) {

		for (size_t i = 0; i < this->m_vBuffers.size(); i++){
			ID3D11Buffer* buffer = this->m_vBuffers[i].m_buffer;
			UINT slot = this->m_vBuffers[i].m_slot;

			if (this->m_type == ST_Vertex) {
				deviceContext->VSSetConstantBuffers(slot, 1, &buffer);

			}
			else if (this->m_type == ST_Pixel) {
				deviceContext->PSSetConstantBuffers(slot, 1, &buffer);
			}
		}
	}

	// duck through the resources
	if (!this->m_vBResources.empty()) {
		for (size_t i = 0; i < this->m_vBResources.size(); i++) {
			BoundResourceRecord &brRecord = this->m_vBResources[i];
			if (brRecord.m_rBoundTexture.Valid()) {
				ID3D11ShaderResourceView * ppResV = *(brRecord.m_rBoundTexture);

				/// @todo a `brRecord.m_desc.BindCount`-al kezdj valamit plz
				if (brRecord.m_desc.BindCount != 1)
					DebugBreak();

				if (m_type == ST_Vertex) {
					deviceContext->VSSetShaderResources(brRecord.m_desc.BindPoint, brRecord.m_desc.BindCount, &ppResV);
				}
				else if (m_type == ST_Pixel) {
					deviceContext->PSSetShaderResources(brRecord.m_desc.BindPoint, brRecord.m_desc.BindCount, &ppResV);
				}
			}
		}
	}
}


void Shader::CompileShader(ID3D11Device * const & device, ID3D10Blob* shaderBuffer)
{
	HRESULT result = 0;
	if (this->m_type == ST_Vertex) {
		// Create the vertex shader from the buffer.
		result = device->CreateVertexShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), NULL, &m_vxShader);
		if (FAILED(result))
		{
			throw EX(VSCrerateException);
		}
	}
	else if (this->m_type == ST_Pixel) {
		// Create the pixel shader from the buffer.
		result = device->CreatePixelShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), NULL, &m_pxShader);
		if (FAILED(result))
		{
			throw EX(FSCrerateException);
		}
	}

	this->BuildReflection(device, shaderBuffer);
}


Shader::ConstantBufferRecord& Shader::operator[](const char * name)
{
	static ConstantBufferRecord null_object;

	if (this->m_mapBuffers.empty()) {
		// no recorded items, keep moving on
		return null_object;
	}

	bufferMap_t::iterator it = this->m_mapBuffers.find(name);
	if (it == this->m_mapBuffers.end()) {
		// no item found, moving on
		return null_object;
	}

	return this->operator[](it->second);
}


Shader::ConstantBufferRecord & FWrender::Shader::operator[](size_t id)
{
	static ConstantBufferRecord null_object;

	if (id >= this->m_vBuffers.size()) {
		return null_object;
	}

	return this->m_vBuffers[id];
}


Shader::BoundResourceRecord & FWrender::Shader::GetBResource(const char * const name)
{
	static BoundResourceRecord null_object;

	bResourceMap_t::iterator it = this->m_mapBResources.find(name);
	if (it == this->m_mapBResources.end()) {
		return null_object;
	}

	return this->GetBResource(it->second);
}


Shader::BoundResourceRecord & FWrender::Shader::GetBResource(size_t id)
{
	static BoundResourceRecord null_object;
	if (id >= this->m_vBResources.size())
	{
		return null_object;
	}
	return this->m_vBResources[id];
}


void Shader::DispatchShaderErrorMessage(ID3D10Blob* errorMessage, LPCWCHAR file, LPCSTR entry)
{
	char* compileErrors = NULL;
	unsigned long bufferSize = 0;
	
	// duck this rainbow
	FILE* fp = NULL;

	std::wstring error_string;

	compileErrors = (char*)(errorMessage->GetBufferPointer());
	bufferSize = errorMessage->GetBufferSize();

	fopen_s(&fp, "shader-error.txt", "w");

	fputs(compileErrors, fp); fputs("\r\n", fp);

	fflush(fp);
	fclose(fp);

	errorMessage->Release();
	errorMessage = 0;

	// @todo add compile errors text 
	throw EX_DETAILS(ShaderException, "See shader-error.txt");
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

	// I should took this mess into a LUT
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


void Shader::BuildReflection(ID3D11Device* device, ID3D10Blob* shaderBuffer)
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
	this->m_mapInputElems.clear(); // push_back(elem);
	// this->m_mapInputElems.insert

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
		
		// ++ copy fucking name
		// this->m_inputNames.push_back(elementDesc.SemanticName);
		// const char* name = this->m_inputNames.back().c_str();
		const char *name = elementDesc.SemanticName;
		
		//this->m_mapInputElems[name] = elem;
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

	// fetch constant buffers
	for (size_t i = 0; i < desc.ConstantBuffers; i++)
	{
		// struct ConstantBufferLayout cbLayout;
		ConstantBufferRecord cbRecord(device, this->m_pReflector->GetConstantBufferByIndex(i));
		this->m_vBuffers.push_back(cbRecord);
		size_t id = this->m_vBuffers.size() - 1;
		this->m_mapBuffers[cbRecord.m_description.Name] = id;
	}

	// fetch samplers
	for (size_t i = 0; i < desc.BoundResources; i++) {
		D3D11_SHADER_INPUT_BIND_DESC brDesc;
		result = this->m_pReflector->GetResourceBindingDesc(i, &brDesc);

		if (FAILED(result)) {
			throw EX(BoundResourceLocateException);
		}

		switch (brDesc.Type) {
			case D3D_SIT_TEXTURE: {
				size_t id = m_vBResources.size();
				m_vBResources.push_back(BoundResourceRecord(brDesc));
				m_mapBResources[brDesc.Name] = id;
			} break;
			case D3D_SIT_SAMPLER: {
				///@ todo something 
			} break;
		}
		// sampler state ??? 
	}
}


// =============================================================================================================================
Shader::ConstantBufferRecord::ConstantBufferRecord() : 
	m_pDC(NULL),
	m_buffer(NULL),
	m_description(),
	m_slot(0) ///@todo slotok 
{
	// DebugBreak();
}


Shader::ConstantBufferRecord::ConstantBufferRecord(ID3D11Device* device, ID3D11ShaderReflectionConstantBuffer *pConstBuffer) :
	ConstantBufferRecord()
{
	// create a dummy object w/o parsing
	if (!device || !pConstBuffer)
		return;

	HRESULT result = 0; 
	
	ZeroMemory(&m_description, sizeof(m_description));
	result = pConstBuffer->GetDesc(&this->m_description);

	if (FAILED(result)) {
		throw EX_DETAILS(ConstantBufferLocateException, "Could not obtain description");
	}

	ID3D11Buffer* buffer = NULL;
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = this->m_description.Size;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&bufferDesc, NULL, &this->m_buffer);
	if (FAILED(result)) {
		throw EX(ConstantBufferCreateException);
	}

	device->GetImmediateContext(&this->m_pDC);

	LOG(TRACE) << "Constant buffer record" << m_description.Name << m_description.Size << m_description.Type;

	// build up cbuffer variables
	for (size_t i = 0; i < m_description.Variables; i++) {
		ID3D11ShaderReflectionVariable* shader_variable = pConstBuffer->GetVariableByIndex(i);
		ConstantBufferElement variable_elem(this, shader_variable);
		m_vConstVars.push_back(variable_elem);
		size_t id = m_vConstVars.size() - 1;
		m_mapConstVars[variable_elem.m_var_desc.Name] = id;
	}
}


void Shader::ConstantBufferRecord::Map()
{
	HRESULT result = 0;

	result = this->m_pDC->Map(this->m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &m_mappedResource);
	if (FAILED(result)) {
		throw EX_DETAILS(ConstantBufferMapException, "Cannot map buffer to a resource");
	}

}


inline void Shader::ConstantBufferRecord::Unmap()
{
	this->m_pDC->Unmap(this->m_buffer, 0);
}


inline void * Shader::ConstantBufferRecord::GetMappedPtr()
{
	return m_mappedResource.pData;
}


void Shader::ConstantBufferRecord::Set(void * data)
{
	this->Map();
	memcpy(this->GetMappedPtr(), data, this->m_description.Size);
	this->Unmap();
}


void Shader::ConstantBufferRecord::Set(void * pData, size_t offset, size_t width)
{
	if (!m_buffer) {
		LOG(TRACE) << "No buffer was created";
		return;
	}

	this->Map();
	memcpy(((unsigned char*)this->GetMappedPtr()) + offset, pData, width);
	this->Unmap();
}


Shader::ConstantBufferElement & FWrender::Shader::ConstantBufferRecord::operator[](const char * name)
{
	static ConstantBufferElement null_object;

	cb_variableMap_t::iterator it = this->m_mapConstVars.find(name);
	if (it == this->m_mapConstVars.end())
		return null_object;

	return this->operator[](it->second);

}


Shader::ConstantBufferElement & FWrender::Shader::ConstantBufferRecord::operator[](size_t id)
{
	static ConstantBufferElement null_object;

	if (id >= m_vConstVars.size())
		throw EX(OutOfBoundsException);

	return this->m_vConstVars[id];
}


// ============================================================================================================
// CBelem

Shader::ConstantBufferElement::ConstantBufferElement():
	m_pBufferRecord(nullptr)
{
	// do nothing here. 
}


Shader::ConstantBufferElement::ConstantBufferElement(Shader::ConstantBufferRecord * parent_record, ID3D11ShaderReflectionVariable * shader_variable) : ConstantBufferElement()
{
	if (!parent_record && !shader_variable)
		throw EX(NullPointerException);

	this->m_pBufferRecord = parent_record;

	HRESULT res = 0;
	
	ZeroMemory(&m_var_desc, sizeof(m_var_desc));
	res = shader_variable->GetDesc(&m_var_desc);

	if (FAILED(res)) {
		throw EX_DETAILS(ConstantBufferLocateException, "Could not obtain description for a cb variable");
	}

	ID3D11ShaderReflectionType* sr_type = shader_variable->GetType();
	
	ZeroMemory(&m_type_desc, sizeof(m_type_desc));
	res = sr_type->GetDesc(&m_type_desc);

	if (FAILED(res)) {
		throw EX_DETAILS(ConstantBufferLocateException, "Could not obtain shader reflection type description");
	}

	LOG(TRACE) << "Shader vairable created " << parent_record->m_description.Name << m_var_desc.Name << m_type_desc.Name;

}


D3D11_SHADER_VARIABLE_DESC & const FWrender::Shader::ConstantBufferElement::GetVarDesc()
{
	static D3D11_SHADER_VARIABLE_DESC null_obj;
	if (!this->m_pBufferRecord)
		return null_obj;

	return this->m_var_desc;
}


D3D11_SHADER_TYPE_DESC & const FWrender::Shader::ConstantBufferElement::GetTypeDesc()
{
	static D3D11_SHADER_TYPE_DESC null_obj;
	if (!this->m_pBufferRecord)
		return null_obj;

	return this->m_type_desc;
}

// ============================================================================================================
// Bindable/Bindig Resource record

FWrender::Shader::BoundResourceRecord::BoundResourceRecord() : 
	m_is_valid(0)
{
	ZeroMemory(&m_desc, sizeof(m_desc));
}


FWrender::Shader::BoundResourceRecord::BoundResourceRecord(D3D11_SHADER_INPUT_BIND_DESC desc)
	: m_desc(desc), m_is_valid(1)
{
	LOG(TRACE) << "BoundResource:" << desc.Name << "type: " << desc.Type << desc.BindPoint 
		<< desc.BindCount << desc.Dimension << desc.NumSamples << desc.ReturnType << desc.uFlags;
}
