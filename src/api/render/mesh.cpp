#include "mesh.h"
#include "texture.h"

#include "../core/exceptions.h"

using namespace FWrender;
using namespace FWdebugExceptions;


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

void FWrender::Mesh::addElement(ID3D11Buffer *pBuffer, UINT stride, UINT offset)
{
	BufferStateDescriptor elem;
	elem.buffer = pBuffer;
	elem.stride = stride;
	elem.offset = offset;
	this->m_buffers.push_back(elem);
}


void FWrender::Mesh::RenderBuffers(ID3D11DeviceContext * deviceContext)
{
	for (size_t i = 0; i<this->m_buffers.size(); ++i)
	{
		BufferStateDescriptor &elem = this->m_buffers[i];

		/// @todo --- ezt
		/// https://msdn.microsoft.com/en-us/library/windows/desktop/ff476456(v=vs.85).aspx
		deviceContext->IASetVertexBuffers(0, 1, &elem.buffer, &elem.stride, &elem.offset); // < megbasz
	}
	
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

	for (int i = 0; i < this->m_buffers.size(); i++) {
		this->m_buffers[i].buffer->Release();
	}

	return;
}


// ========================================================================
# if 0
MeshRef FWrender::SimpleMeshGenerator::operator()(int vertexCount, const float3 * pPosition, int indexCount, const int * pIndices, MeshRef input_mesh)
{
	Mesh* mesh = NULL;
	HRESULT result = 0;
	typedef struct Mesh::VertexType_pos _vertex_t;
	
	// check inputs
	if (!vertexCount || !pPosition || !indexCount || !pIndices)
		throw EX(NullPointerException);
	
	ID3D11Buffer *vertexBuffer = NULL;

	if (input_mesh.Invalid()) {
		mesh = new Mesh();
	}
	else {
		mesh = input_mesh;
	}	this->createIndexBuffer(mesh, indexCount, pIndices);

	// --- 
	_vertex_t* vertices = NULL;

	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData;
	
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	ZeroMemory(&vertexData, sizeof(vertexData));

	// -- create vertex struct buffer + copy 
	vertices = new _vertex_t[vertexCount];		//zero nem kell?

	for (size_t i = 0; i < vertexCount; i++)
	{
		vertices[i].position = pPosition[i];
	}

	// --- create static VBO
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(_vertex_t) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = this->m_device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);
	if (FAILED(result)) { 
		throw EX(CreateIndevBufferException);
	}

	delete[] vertices; vertices = 0;

	mesh->addElement(vertexBuffer, sizeof(_vertex_t), 0);

	return mesh;
}

MeshRef FWrender::SimpleMeshGenerator::operator()(int vertexCount, const float3 *pPosition, const float3 *pNormal, const float2 *pUv, const float3 *pTangent, int indexCount, const int *pIndices, MeshRef input_mesh)
{
	Mesh* mesh = NULL;
	HRESULT result = 0;
	typedef struct Mesh::VertexType_pos_tex _vertex_t;	/// @todo ezt bele lehetne rangatni egy templatebe, nem?

	// check inputs
	if (!vertexCount || !pPosition || !indexCount || !pIndices || !pNormal || !pTangent || !pUv)
		throw EX(NullPointerException);

	ID3D11Buffer *vertexBuffer = NULL;

	if (input_mesh.Invalid()) {
		mesh = new Mesh();
	}
	else {
		mesh = input_mesh;
	}
	this->createIndexBuffer(mesh, indexCount, pIndices);

	// --- 
	_vertex_t* vertices = NULL;

	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData;

	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	ZeroMemory(&vertexData, sizeof(vertexData));

	// -- create vertex struct buffer + copy 
	vertices = new _vertex_t[vertexCount];		//zero nem kell?

	/// todo ezt a kernelt lehetne altalnositani, nem? valahogy? 
	for (size_t i = 0; i < vertexCount; i++)
	{
		vertices[i].position = pPosition[i];
		vertices[i].normal = pNormal[i];
		vertices[i].tangent= pTangent[i];
		vertices[i].texture = pUv[i];
	}

	// --- create static VBO
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(_vertex_t) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = this->m_device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);
	if (FAILED(result)) {
		throw EX(CreateIndevBufferException);
	}

	delete[] vertices; vertices = 0;

	mesh->addElement(vertexBuffer, sizeof(_vertex_t), 0);

	return mesh;
}

void FWrender::SimpleMeshGenerator::createIndexBuffer(MeshRef mesh, int indexCount, const int * pIndices)
{
	ID3D11Buffer *indexBuffer = NULL;

	HRESULT result = 0;

	D3D11_BUFFER_DESC 	indexBufferDesc;
	D3D11_SUBRESOURCE_DATA 	indexData;

	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
	ZeroMemory(&indexData, sizeof(indexData));

	// check inputs
	if (mesh.Invalid() || !indexCount || !pIndices)
		throw EX(NullPointerException);	

	// --- 

	// -- indices buffer + copy 
	ULONG* indices = NULL;
	indices = new ULONG[indexCount];	// zero nem kell?
	
	// !copy
	for (size_t i = 0; i < indexCount; i++)
	{
		indices[i] = pIndices[i];
	}
	
	// --- create static index buffer
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;
	
	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	
	// Create the index buffer.
	result = this->m_device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);
	if (FAILED(result)) { 
		throw EX(CreateIndevBufferException);
	}
	
	delete[] indices; indices = 0;

	mesh->addIndices(indexBuffer, indexCount);
}
#endif 