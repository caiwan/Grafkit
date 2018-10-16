#pragma once

#include <list>
#include <map>

#include "common.h"

#include "utils/persistence/persistence.h"

namespace Grafkit
{
    /**
    Stores basic data for a piece of mesh
    */
    class Mesh : public Object
    {
    public:
        Mesh();

        Mesh(const Mesh& other) = delete;
        Mesh& operator=(const Mesh& other) = delete;

        virtual ~Mesh();

        virtual void UpdateMesh(double time);
        void RenderMesh(ID3D11DeviceContext* deviceContext) const;

        void AddPointer(std::string inputName, uint32_t length, const void* pointer); // TODO because it copes the underlying data, It should have a more meaningful name instead
        void SetIndices(uint32_t vertexCount, uint32_t indexCount, const uint32_t* const indices);
        void Build(ID3D11Device*const& device, ShaderRef& vertexShader);

        size_t GetIndexCount() const { return m_indices.size(); }

        const void* GetPointer(std::string inputName);
        const uint32_t* GetIndices() const;

    protected:
        void Shutdown() override;

        struct BufferStateDescriptor
        {
            ID3D11Buffer* buffer;
            UINT stride, offset;

            BufferStateDescriptor() : buffer(nullptr)
                , stride(0)
                , offset(0) {
            }
        };

        SERIALIZE(Grafkit::Mesh, 1, ar)
        {
            Object::Serialize(ar);
            ar & m_vertexCount & m_indices & m_vertices;
        }

    private:
        ID3D11Buffer* m_indexBuffer;
        BufferStateDescriptor m_buffer;
        uint32_t m_vertexCount;
        std::vector<uint32_t> m_indices;
        std::map<std::string, std::vector<uint8_t>> m_vertices;
    };

    typedef Ref<Mesh> MeshRef;
}
