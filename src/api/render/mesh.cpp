#include "mesh.h"
#include "texture.h"

#include "exceptions.h"

#include "struct_pack.h"

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

void FWrender::Mesh::Render(ID3D11DeviceContext * dev)
{
	dev->IASetVertexBuffers(0, 1, &this->m_buffer.buffer, &this->m_buffer.stride, &this->m_buffer.offset);
	dev->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	dev->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	dev->DrawIndexed(this->m_indexCount, 0, 0);
}

void FWrender::Mesh::Shutdown()
{
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	//for (int i = 0; i < this->m_buffers.size(); i++) {
	//	this->m_buffers[i].buffer->Release();
	//}

	if (this->m_buffer.buffer)
		this->m_buffer.buffer->Release();

	return;
}

void FWrender::Mesh::addElement(ID3D11Buffer *pBuffer, UINT stride, UINT offset)
{
	BufferStateDescriptor elem;
	elem.buffer = pBuffer;
	elem.stride = stride;
	elem.offset = offset;
	// this->m_buffers.push_back(elem);
	this->m_buffer = elem;
}


//void FWrender::Mesh::RenderBuffers(ID3D11DeviceContext * deviceContext)
//{
//	for (size_t i = 0; i<this->m_buffers.size(); ++i)
//	{
//		BufferStateDescriptor &elem = this->m_buffers[i];
//
//		/// @todo --- ezt
//		/// https://msdn.microsoft.com/en-us/library/windows/desktop/ff476456(v=vs.85).aspx
//		deviceContext->IASetVertexBuffers(0, 1, &elem.buffer, &elem.stride, &elem.offset); // < megbasz
//	}
//	
//	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
//	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//	
//	// FYI https://msdn.microsoft.com/en-us/library/windows/desktop/ff728726(v=vs.85).aspx
//
//	return;
//}


// ========================================================================
FWrender::SimpleMeshGenerator::SimpleMeshGenerator(ID3D11Device * device, ShaderRef shader)
	: m_device(device), m_shader(shader)
{
}

MeshRef FWrender::SimpleMeshGenerator::operator()(size_t vertexCount, size_t indexCount, const int* indices, MeshRef mesh_input)
{
	if (this->m_shader.Invalid())
		throw EX(NullPointerException);

	HRESULT result = 0;

	// obtain input layout elements, and collect pointers that were set before
	size_t elem_count = this->m_shader->getILayoutElemCount();

	FWutils::StructPack packer;
	packer.setFieldPadding(16);
	//input_packer.setRecordPadding(16);


	for (size_t i = 0; i < elem_count; ++i)
	{
		Shader::InputElementRecord &record = this->m_shader->getILayoutElem(i);
		int field_id = packer.addField(record.width);

		mapPtr_t::iterator it = this->m_mapPtr.find(record.desc.SemanticName);
		if (it != this->m_mapPtr.end())
		{
			packer.addPointer(field_id, it->second, 0, record.width);
		}
	}

	MeshRef mesh;
	if (mesh_input.Valid()) mesh = mesh_input; else mesh = new Mesh();

	void *vertex_buffer_data = packer(vertexCount);

	// create buffer + index
	ID3D11Buffer *vertexBuffer = NULL;

	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData;

	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	ZeroMemory(&vertexData, sizeof(vertexData));

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = packer.getBufferSize();
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertex_buffer_data;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = this->m_device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);
	if (FAILED(result)) {
		throw EX(CreateIndevBufferException);
	}

	// delete[] vertex_data; //vertex_data = 0;
	/// @todo delete vertex_buffer_data 

	this->createIndexBuffer(mesh, indexCount, indices);
	mesh->addElement(vertexBuffer, packer.getRecordWidth(), 0);

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
