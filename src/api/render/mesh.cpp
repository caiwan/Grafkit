#include "stdafx.h"

#include "mesh.h"

#include "texture.h"

#include "utils/struct_pack.h"
#include "utils/persistence/persistence.h"

using namespace Grafkit;
using namespace FWdebugExceptions;

PERSISTENT_IMPL(Grafkit::Mesh);

void Grafkit::Mesh::_serialize(Archive & ar)
{
	PERSIST_FIELD(ar, m_vertexCount);
	PERSIST_VECTOR(ar, m_indices, m_indexCount);

	unsigned int ptr_count = this->m_vertexPointers.size();

	PERSIST_FIELD(ar, ptr_count);

	// store pointer table
	if (ar.IsStoring()) {
		for (auto it = m_vertexPointers.begin(); it != m_vertexPointers.end(); ++it) {
			const UCHAR* ptr = (UCHAR*)it->data;
			size_t size = it->length;
			std::string name = it->name.c_str();
			PERSIST_STRING(ar, name);
			PERSIST_VECTOR(ar, ptr, size);
		}
	}
	// load pointer table
	else {
		for (size_t i = 0; i < ptr_count; i++) {
			UCHAR* ptr = nullptr;
			size_t size = 0; 
			std::string name;
			PERSIST_STRING(ar, name);
			PERSIST_VECTOR(ar, ptr, size);
			
			this->AddPointer(name, size, ptr);
			delete[] ptr;	///TODO: ez itt felesleges, eleg lenne egyszer torolni csak
		}
	}
}

// ==================================================================
Grafkit::Mesh::Mesh() :
	m_buffer(),
	m_indexBuffer(nullptr),
	m_indexCount(0),
	m_vertexCount(0)
{
}

Grafkit::Mesh::Mesh(const Mesh & mesh)
{
	///@todo TBD
}

Grafkit::Mesh::~Mesh()
{
	this->Shutdown();
}

void Grafkit::Mesh::UpdateMesh(double time)
{
}

void Grafkit::Mesh::RenderMesh(ID3D11DeviceContext * dev)
{
	dev->IASetVertexBuffers(0, 1, &this->m_buffer.buffer, &this->m_buffer.stride, &this->m_buffer.offset);
	dev->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	dev->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	dev->DrawIndexed(this->m_indexCount, 0, 0);
}

void Grafkit::Mesh::AddPointer(std::string inputName, size_t length, const void * pointer)
{
	// create and copy ptr
	vertex_pointer_t vertexPointer;
	vertexPointer.name = inputName;
	vertexPointer.length = length;
	vertexPointer.data = new UCHAR[length];

	memcpy_s(vertexPointer.data, vertexPointer.length, pointer, length);

	this->m_vertexPointers.push_back(vertexPointer);
	this->m_mapPtr[inputName] = vertexPointer.data;
}

void Grafkit::Mesh::SetIndices(size_t vertexCount, size_t indexCount, const int * const indices)
{
	m_indexCount = indexCount;
	m_vertexCount = vertexCount;
	m_indices = new UINT[indexCount];
	for (size_t i = 0; i < indexCount; i++)
		m_indices[i] = indices[i];
}

void Grafkit::Mesh::Build(ID3D11Device * const & device, ShaderRef & shader)
{
	HRESULT result = 0;

	if (m_mapPtr.empty())
		return;

	size_t elem_count = shader->GetILayoutElemCount();

	Grafkit::StructPack packer;

	if (shader.Invalid()) {
		throw EX_DETAILS(NullPointerException, "Nincs shader betoltve");
	}

	for (size_t i = 0; i < elem_count; ++i)
	{
		Shader::InputElementRecord &record = shader->getILayoutElem(i);
		int field_id = packer.addField(record.width);

		auto it = this->m_mapPtr.find(record.desc.SemanticName);
		if (it != this->m_mapPtr.end())
		{
			packer.addPointer(field_id, it->second, 0, record.width);
		}
	}

	void *vertex_buffer_data = packer(m_vertexCount);

	// --- Create vertex buffer
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
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);
	if (FAILED(result)) {
		throw EX(CreateIndevBufferException);
	}

	packer.dealloc(vertex_buffer_data);

	// set vertex buffer
	this->m_buffer.buffer = vertexBuffer;
	this->m_buffer.stride = packer.getRecordWidth();
	this->m_buffer.offset = 0;

	// --- Create index buffer
	ID3D11Buffer *indexBuffer = NULL;

	D3D11_BUFFER_DESC 	indexBufferDesc;
	D3D11_SUBRESOURCE_DATA 	indexData;

	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
	ZeroMemory(&indexData, sizeof(indexData));

	// static index buffer
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(UINT) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = m_indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);
	if (FAILED(result)) {
		throw EX(CreateIndevBufferException);
	}

	this->m_indexBuffer = indexBuffer;
}

void Grafkit::Mesh::Shutdown()
{
	RELEASE(m_indexBuffer);
	RELEASE(m_buffer.buffer);

	for (auto it = this->m_vertexPointers.begin(); it != m_vertexPointers.end(); ++it) {
		delete[] it->data;
	}

	delete[] m_indices;
}
