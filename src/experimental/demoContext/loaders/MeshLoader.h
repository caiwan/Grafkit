#pragma once
#include <vector>

#include "core/resource.h"
#include "resource/ResourceBuilder.h"

struct aiScene;

namespace Assimp
{
    class Importer;
}

namespace GkDemo
{
    struct MeshLoadParams
    {
        // ...
        std::string filename;
        std::string typeHint;
        // TODO idx || select nem somehow if needed
        //uint32_t meshIndex; 

        template <class A>
        void Serialize(A& a) { a & filename & typeHint /*& meshIndex*/; }
    };

    class MeshLoader : public Grafkit::ResourceBuilder<Grafkit::Mesh, MeshLoadParams>
    {
    public:
        MeshLoader() {
        }

        explicit MeshLoader(const MeshLoadParams& params)
            : ResourceBuilder<Grafkit::Mesh, MeshLoadParams>(params) {
        }

        MeshLoader(const std::string& name, const std::string& uuid, const MeshLoadParams& params)
            : ResourceBuilder<Grafkit::Mesh, MeshLoadParams>(name, uuid, params) {
        }

        void Load(Grafkit::IResourceManager* const& resman, Grafkit::IResource* source) override;
        void Initialize(Grafkit::Renderer& render, Grafkit::IResourceManager* const& resman, Grafkit::IResource* source) override;

        SERIALIZE(GkDemo::MeshLoader, 1, ar)
        {
            assert(0);
        }

        static void LoadMeshes(std::vector<Grafkit::MeshRef>& meshes, const aiScene const*& souceScene);

    };
}