#include "model.h"
#include "texture.h"

using namespace FWrender;

Model::Model()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
}


Model::Model(const Model& other)
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
}


Model::~Model()
{
	this->Shutdown();
}

// ------------------------------------------------------------------

ID3D11ShaderResourceView* Model::GetTexture() 
{ 
	return m_texture ? m_texture->GetTexture() : 0; 
}

// ------------------------------------------------------------------

bool Model::Initialize(
	ID3D11Device* device,
	FWrender::TextureRef texture,
	int indicesCount, const int* pIndices,
	int verticesCount, const float3* position, const float2* uv
	)
{
	HRESULT result = 0;

	///@todo check params for zero/nullptr

	struct VertexType* vertices = NULL;
	ULONG* indices = NULL;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	//HRESULT result = 0;

	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

	ZeroMemory(&vertexData, sizeof(vertexData));
	ZeroMemory(&indexData, sizeof(indexData));

	// -- create vertex struct buffer + copy 
	this->m_vertexCount = verticesCount;
	vertices = new struct VertexType[m_vertexCount];

	for (size_t i = 0; i < m_vertexCount; i++)
	{
		vertices[i].position = position[i];
		vertices[i].texture = uv[i];
	}

	// -- indices buffer + copy 
	this->m_indexCount = indicesCount;
	indices = new ULONG[m_indexCount];

	for (size_t i = 0; i < m_indexCount; i++)
	{
		indices[i] = pIndices[i]; 
	}


	// --- create static VBO
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result)){ /**@todo throw exception */ return false; }


	// --- create static index buffer
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result)) { /**@todo throw exception */ return false; }
	
	delete[] vertices; vertices = 0;
	delete[] indices; indices = 0;

	// --- setup texture and material
	this->m_texture = texture;

	return true;
}

void Model::Shutdown()
{
	ShutdownBuffers();
	return;
}


void Model::Render(ID3D11DeviceContext* deviceContext)
{
	RenderBuffers(deviceContext);
	return;
}

void Model::ShutdownBuffers()
{
	// Release the index buffer.
	if(m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if(m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}


void Model::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(VertexType); 
	offset = 0;
	
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}
