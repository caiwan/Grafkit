#include "model.h"
#include "texture.h"

using namespace FWrender;


// ==================================================================
FWrender::Mesh::Mesh()
{
	//m_vertexBuffer = 0;
	m_indexBuffer = 0;

}

FWrender::Mesh::Mesh(const Mesh & mesh)
{
	///@todo TBD
}

FWrender::Mesh::~Mesh()
{
	this->Shutdown();
}

void FWrender::Mesh::Render(ID3D11DeviceContext * deviceContext)
{
	RenderBuffers(deviceContext);
}

void FWrender::Mesh::Shutdown()
{
	this->ShutdownBuffers();
}


//void FWrender::Mesh::CreateVertexBuffer(ID3D11Device * device, int vertexCount, const float3 * position, const float2 * uv)
//{
//	///@todo ha created, akkor delete + recreate, vagy felulir valahogy 
//
//	HRESULT result = 0;
//
//	struct VertexType* vertices = NULL;
//
//	D3D11_BUFFER_DESC vertexBufferDesc;
//	D3D11_SUBRESOURCE_DATA vertexData;
//	
//	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
//	ZeroMemory(&vertexData, sizeof(vertexData));
//
//	// -- create vertex struct buffer + copy 
//	this->m_vertexCount = vertexCount;
//	vertices = new struct VertexType[m_vertexCount];
//
//	for (size_t i = 0; i < m_vertexCount; i++)
//	{
//		vertices[i].position = position[i];
//		vertices[i].texture = uv[i];
//	}
//
//	// --- create static VBO
//	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
//	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
//	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//	vertexBufferDesc.CPUAccessFlags = 0;
//	vertexBufferDesc.MiscFlags = 0;
//	vertexBufferDesc.StructureByteStride = 0;
//
//	// Give the subresource structure a pointer to the vertex data.
//	vertexData.pSysMem = vertices;
//	vertexData.SysMemPitch = 0;
//	vertexData.SysMemSlicePitch = 0;
//
//	// Now create the vertex buffer.
//	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
//	if (FAILED(result)) { /**@todo throw exception */ return; }
//
//	delete[] vertices; vertices = 0;
//
//}

//void FWrender::Mesh::CreateIndexBuffer(ID3D11Device * device, int indexCount, const int * pIndices)
//{
//	///@todo ha created, akkor delete + recreate, vagy felulir valahogy 
//
//	HRESULT result = 0;
//
//	///@todo check params for zero/nullptr
//
//	ULONG* indices = NULL;
//	D3D11_BUFFER_DESC 	indexBufferDesc;
//	D3D11_SUBRESOURCE_DATA 	indexData;
//
//	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
//	ZeroMemory(&indexData, sizeof(indexData));
//
//	// -- indices buffer + copy 
//	this->m_indexCount = indexCount;
//	indices = new ULONG[m_indexCount];
//
//	for (size_t i = 0; i < m_indexCount; i++)
//	{
//		indices[i] = pIndices[i];
//	}
//
//	// --- create static index buffer
//	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
//	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
//	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
//	indexBufferDesc.CPUAccessFlags = 0;
//	indexBufferDesc.MiscFlags = 0;
//	indexBufferDesc.StructureByteStride = 0;
//
//	// Give the subresource structure a pointer to the index data.
//	indexData.pSysMem = indices;
//	indexData.SysMemPitch = 0;
//	indexData.SysMemSlicePitch = 0;
//
//	// Create the index buffer.
//	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
//	if (FAILED(result)) { /**@todo throw exception */ return; }
//
//	delete[] indices; indices = 0;
//}

void FWrender::Mesh::RenderBuffers(ID3D11DeviceContext * deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(VertexType);
	offset = 0;

	/// @todo --- ezt
	/// https://msdn.microsoft.com/en-us/library/windows/desktop/ff476456(v=vs.85).aspx
	deviceContext->IASetVertexBuffers(0, this->m_buffers.size(), &this->m_buffers[0], &stride, &offset);
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	// FYI https://msdn.microsoft.com/en-us/library/windows/desktop/ff728726(v=vs.85).aspx

	return;
}

void FWrender::Mesh::ShutdownBuffers()
{
	// Release the index buffer.
	
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}
	// Release the vertex buffer.
	/*
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}
	*/
	for (int i = 0; i < this->m_buffers.size(); i++) {
		this->m_buffers[i]->Release();
	}

	return;
}