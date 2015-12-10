
#include <cstdio>

#include "shader.h"

#include "../math/matrix.h"

using namespace std;
using namespace FWrender;

using namespace FWdebugExceptions;

using FWmath::Matrix;

// =============================================================================================================================

Shader::Shader() : 
	m_pShader(NULL),
	m_vShader(NULL),
	m_pReflector(NULL),
	m_layout(NULL)
{
}

FWrender::Shader::~Shader()
{
	this->Shutdown();
}

//void FWrender::Shader::LoadFromFile(ID3D11Device * device, LPCSTR vsEntry, LPCSTR fsEntry, LPCWCHAR vsFile, LPCWCHAR fsFile)
void Shader::LoadFromFile(ID3D11Device* device, LPCSTR entry, LPCWCHAR file, ShaderType_e type)
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
			throw new EX(MissingShaderExcepotion);
		}
	}

	if (type == ST_Vertex) {
		// Create the vertex shader from the buffer.
		result = device->CreateVertexShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), NULL, &m_vShader);
		if (FAILED(result))
		{
			throw new EX(VSCrerateException);
		}
	}
	else if (type == ST_Pixel){
		// Create the pixel shader from the buffer.
		result = device->CreatePixelShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), NULL, &m_pShader);
		if (FAILED(result))
		{
			throw new EX(FSCrerateException);
		}
	}

	this->BuildReflection(device, shaderBuffer);

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	shaderBuffer->Release();
	shaderBuffer = NULL;
}

void FWrender::Shader::Shutdown()
{
	// why is this empty 
	// fuck the constant buffers around
}

void FWrender::Shader::Render(ID3D11DeviceContext * deviceContext)
{

	// fuck the constant buffers around
	if (!this->m_mapBuffers.empty()) {
		bufferMap_t::iterator it;
		for (it = this->m_mapBuffers.begin(); it != this->m_mapBuffers.end(); it++) {
			if (it->second.m_buffer) {
				ID3D11Buffer* buffer = it->second.m_buffer;
				UINT slot = 1;

				if (this->m_type == ST_Vertex) {
					deviceContext->VSGetConstantBuffers(slot, 1, &buffer);

				}
				else if (this->m_type == ST_Pixel) {
					deviceContext->PSGetConstantBuffers(slot, 1, &buffer);
				}
			}
		}
	}
	// fuck through the resources

	// go the fuck off 
	if (this->m_type == ST_Vertex) {
		// fuck off the layout
		deviceContext->IASetInputLayout(m_layout);
		deviceContext->VSSetShader(m_vShader, NULL, 0);
	}
	else if (this->m_type == ST_Pixel) {
		deviceContext->PSSetShader(m_pShader, NULL, 0);
	}

}

FWrender::Shader::ConstantBufferRecord& FWrender::Shader::operator[](const char * name)
{
	if (this->m_mapBuffers.empty()) {
		// no recorded items, keep moving on
		return ConstantBufferRecord();
	}

	bufferMap_t::iterator it = this->m_mapBuffers.find(name);
	if (it == this->m_mapBuffers.end()) {
		// no item found, moving on
		return ConstantBufferRecord();
	}

	return it->second;
}

void FWrender::Shader::DispatchShaderErrorMessage(ID3D10Blob* errorMessage, LPCWCHAR file, LPCSTR entry)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	
	// ezt itt ki kell kurni ugy ahogy van
	FILE* fp = NULL;

	std::wstring error_string();

	compileErrors = (char*)(errorMessage->GetBufferPointer());
	bufferSize = errorMessage->GetBufferSize();

	fopen_s(&fp, "shader-error.txt", "w");

	fputs(compileErrors, fp); fputs("\r\n", fp);

	fflush(fp);
	fclose(fp);

	errorMessage->Release();
	errorMessage = 0;

	// @todo add compile errors text 
	throw new EX_DETAILS(ShaderException, L"See shader-error.txt");
}

void FWrender::Shader::BuildReflection(ID3D11Device* device, ID3D10Blob* shaderBuffer)
{
	HRESULT result = 0;

	// create reflection interface 
	result = D3DReflect(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&m_pReflector);
	if (FAILED(result)) {
		/// @todo throw exception
	}

	D3D11_SHADER_DESC desc;
	this->m_pReflector->GetDesc(&desc);

	// fetch input descriptors
	// input layouts? 
	for (size_t i = 0; i < desc.InputParameters; i++)
	{
		D3D11_SIGNATURE_PARAMETER_DESC input_desc;
		this->m_pReflector->GetInputParameterDesc(i, &input_desc);
		
		// --- 
		https://takinginitiative.wordpress.com/2011/12/11/directx-1011-basic-shader-reflection-automatic-input-layout-creation/

		InputElementRecord elem;

		// fill out input element desc
		D3D11_INPUT_ELEMENT_DESC elementDesc;
		std::vector<D3D11_INPUT_ELEMENT_DESC> elements;
		elementDesc.SemanticName = input_desc.SemanticName;
		elementDesc.SemanticIndex = input_desc.SemanticIndex;
		elementDesc.InputSlot = 0;
		elementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;	
		elementDesc.InstanceDataStepRate = 0;

		elem.desc = elementDesc;

		// determine DXGI format
		// https://msdn.microsoft.com/en-us/library/windows/desktop/ff728727(v=vs.85).aspx

		//  D3D11Helper.GetFormat
		// http://sharpdx.org/forum/5-api-usage/2136-basic-shader-reflection-automatic-input-layout-creation
		if (input_desc.Mask == 1)
		{
			if (input_desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) { 
				elementDesc.Format = DXGI_FORMAT_R32_UINT; 
				elem.width = 4;
			}
			else if (input_desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32){ 
				elementDesc.Format = DXGI_FORMAT_R32_SINT;
				elem.width = 4;
			}
			else if (input_desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) {
				elementDesc.Format = DXGI_FORMAT_R32_FLOAT;
				elem.width = 4;
			}
		}
		else if (input_desc.Mask <= 3)
		{
			if (input_desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) {
				elementDesc.Format = DXGI_FORMAT_R32G32_UINT;
			}
			else if (input_desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) {
				elementDesc.Format = DXGI_FORMAT_R32G32_SINT;
			}
			else if (input_desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) {
				elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
			}
		}
		else if (input_desc.Mask <= 7)
		{
			if (input_desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) {
				elementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
				elem.width = 8;
			}
			else if (input_desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) {
				elementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
				elem.width = 8;
			}
			else if (input_desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) {
				elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
				elem.width = 8;
			}
		}
		else if (input_desc.Mask <= 15)
		{
			if (input_desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) {
				elementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
				elem.width = 16;
			}
			else if (input_desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) {
				elementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
				elem.width = 16;
			}
			else if (input_desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) {
				elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
				elem.width = 16;
			}
		}
		elements.push_back(elementDesc);
		this->m_mapInputElems[input_desc.SemanticName] = elem;

		if (this->m_type == ST_Vertex) {
			result = device->CreateInputLayout(&elements[0], elements.size(), shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), &this->m_layout);

			if (FAILED(result)) {
				/// @todo throw exceptions 
			}

		}

	}

	// fetch output desctiptor
	/// @todo tudumm 
	for (size_t i = 0; i < desc.OutputParameters; i++)
	{
		D3D11_SIGNATURE_PARAMETER_DESC out_desc;
		this->m_pReflector->GetOutputParameterDesc(i, &out_desc);

		// ... 
		///@todo fetch through shit 
	}

	// fetch constant buffers
	for (size_t i = 0; i < desc.ConstantBuffers; i++)
	{
		// struct ConstantBufferLayout cbLayout;
		ConstantBufferRecord cbRecord(device, this->m_pReflector->GetConstantBufferByIndex(i));
		this->m_mapBuffers[cbRecord.m_description.Name] = cbRecord;
	}
	
	this->m_pReflector->Release();
	this->m_pReflector = NULL;
}

// =============================================================================================================================

FWrender::Shader::ConstantBufferRecord::ConstantBufferRecord(ID3D11Device* device, ID3D11ShaderReflectionConstantBuffer *pConstBuffer) :
	// m_pDC(NULL),
	// m_parent(NULL),
	m_buffer(NULL),
	m_description()
{
	// create a dummy object w/o parsing
	if (!device || !pConstBuffer)
		return;

	HRESULT result = 0; 
	result = pConstBuffer->GetDesc(&this->m_description);

	if (FAILED(result)) {
		throw EX_DETAILS(ConstantBufferLocateException, L"Could not obtain description");
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

	/// @todo fetch through even more shit 
}

void FWrender::Shader::ConstantBufferRecord::set(void * data)
{
	if (!data || !m_buffer)
		return;

	HRESULT result = 0;
	D3D11_MAPPED_SUBRESOURCE MappedResource;

	result = this->m_pDC->Map(this->m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	if (FAILED(result)) {
		throw EX_DETAILS(ConstantBufferLocateException, L"Cannot map resource");
	}

	memcpy(MappedResource.pData, data, this->m_description.Size);

	this->m_pDC->Unmap(this->m_buffer, 0);
}
