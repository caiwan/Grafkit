#pragma once
#include <vector>

#include "core/resource.h"
#include "resource/ResourceBuilder.h"

struct aiScene;

namespace Grafkit
{
    class Mesh;
    typedef Ref<Mesh> MeshRef;
}

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

    class MeshLoader : public Grafkit::ResourceLoader<Grafkit::Mesh, MeshLoadParams>
    {
    public:

        MeshLoader(const std::string& id, const MeshLoadParams& params)
            : ResourceLoader<Grafkit::Mesh, MeshLoadParams>(id, params) {
        }


    protected:
        void Load(const std::shared_ptr<Grafkit::IResource>& resource, Grafkit::ResourceManager& resourceManager) override;
        void Initialize(const std::shared_ptr<Grafkit::IResource>& resource, Grafkit::ResourceManager& resourceManager) override;
    
        static void LoadMeshes(std::vector<Grafkit::MeshRef>& meshes, const aiScene const*& souceScene);

    };
}