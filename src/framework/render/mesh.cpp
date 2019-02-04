#include "stdafx.h"

#include "mesh.h"

#include "texture.h"
#include "shader.h"

#include "utils/struct_pack.h"
#include "utils/persistence/persistence.h"
#include <iterator>

PERSISTENT_IMPL(Grafkit::Mesh);

using namespace Grafkit;

// ==================================================================
Mesh::Mesh() :
    m_indexBuffer(nullptr)
    , m_buffer()
    , m_vertexCount(0) {
}

void Mesh::UpdateMesh(double time) {
}

void Mesh::RenderMesh(Renderer& render) const
{
    auto& dev = render.GetDeviceContext();
    dev->IASetVertexBuffers(0, 1, this->m_buffer.buffer.GetAddressOf(), &this->m_buffer.stride, &this->m_buffer.offset);
    dev->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    dev->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    dev->DrawIndexed(GetIndexCount(), 0, 0);
}

// TODO: Rename: ->CopyVertexAttribute
void Mesh::AddPointer(std::string inputName, uint32_t length, const void* pointer)
{
    std::vector<uint8_t> vertexData;
    vertexData.reserve(length);
    copy(reinterpret_cast<const uint8_t*>(pointer), reinterpret_cast<const uint8_t*>(pointer) + length, back_inserter(vertexData));
    this->m_vertices[inputName] = vertexData;
}

// TODO: Rename: ->CopyAddAttribute
void Mesh::SetIndices(uint32_t vertexCount, uint32_t indexCount, const uint32_t*const indices)
{
    m_vertexCount = vertexCount;
    m_indices.clear();
    m_indices.reserve(indexCount);
    copy(indices, indices + indexCount, back_inserter(m_indices));
}

void Mesh::Build(Renderer& render, const ShaderRes& shader)
{
    assert(shader);
    HRESULT result = 0;

    if (m_vertices.empty())
        return;

    uint32_t elem_count = shader->GetILayoutElemCount();

    StructPack packer;


    for (uint32_t i = 0; i < elem_count; ++i)
    {
        Shader::InputElementRecord record = shader->GetILayoutElem(i);
        int field_id = packer.addField(record.width);

        auto it = this->m_vertices.find(record.name);
        if (it != this->m_vertices.end()) { packer.addPointer(field_id, it->second.data(), 0, record.width); }
    }

    void* vertex_buffer_data = packer(m_vertexCount);

    // --- Create vertex buffer
    ComPtr<ID3D11Buffer> vertexBuffer = nullptr;

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
    result = render->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);
    if (FAILED(result)) { throw std::runtime_error("Could not create index buffer"); }

    packer.dealloc(vertex_buffer_data);

    // set vertex buffer
    this->m_buffer.buffer = vertexBuffer;
    this->m_buffer.stride = packer.getRecordWidth();
    this->m_buffer.offset = 0;

    // --- Create index buffer
    ComPtr<ID3D11Buffer> indexBuffer = nullptr;

    D3D11_BUFFER_DESC indexBufferDesc = {};
    D3D11_SUBRESOURCE_DATA indexData = {};

    //ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
    //ZeroMemory(&indexData, sizeof(indexData));

    // static index buffer
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(UINT) * GetIndexCount();
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.StructureByteStride = 0;

    // Give the subresource structure a pointer to the index data.
    indexData.pSysMem = m_indices.data();
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    // Create the index buffer.
    result = render->CreateBuffer(&indexBufferDesc, &indexData, indexBuffer.GetAddressOf());
    if (FAILED(result)) { throw std::runtime_error("Could not create vertex buffer"); }

    this->m_indexBuffer = indexBuffer;
}

const void* Mesh::GetPointer(const std::string& inputName)
{
    auto it = m_vertices.find(inputName);
    if (it != m_vertices.end()) { return it->second.data(); }

    return nullptr;
}

const uint32_t* Mesh::GetIndices() const { return m_indices.data(); }
