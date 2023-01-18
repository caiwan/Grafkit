#pragma once
#include <list>

#include "utils/exceptions.h"

#include "utils/resource.h"
#include "utils/ResourceBuilder.h"

class aiScene;

namespace GkDemo
{
    class IMeshLoader: public Grafkit::IResourceBuilder
    {
    public:

        IMeshLoader(const std::string& name, const std::string& sourcename, const std::string& uuid)
            : IResourceBuilder(name, sourcename, uuid) {
        }

        Grafkit::IResource* NewResource() override;
        void Load(Grafkit::IResourceManager* const& resman, Grafkit::IResource* source) override = 0;

    protected:
        const aiScene* LoadAiScene(Grafkit::IResourceManager*const& resman) const;

        static void LoadMeshes(std::list<Grafkit::MeshRef>& meshes, const aiScene const*& souceScene);

    };


    class MeshOBJLoader : public IMeshLoader
    {
    public:

        MeshOBJLoader(const std::string& name, const std::string& sourcename, const std::string& persistName, const std::string& uuid = "")
            : IMeshLoader(name, sourcename, uuid)
            , m_persistName(persistName) {
        }

        void Load(Grafkit::IResourceManager* const& resman, Grafkit::IResource* source) override;
    private:
        std::string m_persistName;
    };
}

// exceptions
DEFINE_EXCEPTION(MeshLoaderException, 9001, "Error while parsing file via ASSIMP");
