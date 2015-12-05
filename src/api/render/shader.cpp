
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
	m_vShader(NULL)
{
}

FWrender::Shader::~Shader()
{
	this->Shutdown();
}

//void FWrender::Shader::LoadFromFile(ID3D11Device * device, LPCSTR vsEntry, LPCSTR fsEntry, LPCWCHAR vsFile, LPCWCHAR fsFile)
void Shader::LoadFromFile(ID3D11Device* device, LPCSTR entry, LPCWCHAR file, ShaderType_e type)
{
	HRESULT result;
	ID3D10Blob* errorMessage;

	ID3D10Blob* shaderBuffer;

	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;

	// input checking
	if (!entry) throw new EX(NullPointerException);
	if (!file) throw new EX(NullPointerException);


	// Initialize the pointers this function will use to null.
	errorMessage = 0;
	shaderBuffer = 0;

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

	// create reflection interface 
	result = D3DReflect(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&m_pReflector);
	if (FAILED(result)) {
		/// @todo throw exception
	}

	this->BuildReflection(device);

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
	// fuck off the layout

	// fuck through the resources

	// go the fuck off 
	if (this->m_type == ST_Vertex) {
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

void FWrender::Shader::BuildReflection(ID3D11Device* device)
{
	HRESULT result = 0;
	D3D11_SHADER_DESC desc;
	this->m_pReflector->GetDesc(&desc);

	// fetch input descriptors
	// input layouts? 
	for (size_t i = 0; i < desc.InputParameters; i++)
	{
		D3D11_SIGNATURE_PARAMETER_DESC input_desc;
		this->m_pReflector->GetInputParameterDesc(i, &input_desc);

		// ... 
		///@todo fetch through shit 
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
	if (!m_buffer)
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
