#include"MeshLoader.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/matrix4x4.h"

#include "assimp/vector2.h"
#include "assimp/vector3.h"
#include "assimp/quaternion.h"

#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/material.h"
#include "assimp/mesh.h"
#include "assimp/matrix4x4.h"
#include "assimp/matrix3x3.h"

#include "stdafx.h" // az assimp sajat memoria basztatoja miatt kell lerakni ide, a sajat memoria basztatonk miatt

#include "utils/asset/AssetFactory.h"
#include "core/resource.h"
#include "resource/ResourceManager.h"

#include "utils/persistence/archive.h"
#include "utils/persistence/persistence.h"

#include "math/matrix.h"
#include "render/mesh.h"

#define MAX_TEXCOORD_COUNT 2


// TODO -> Library

namespace
{
    Grafkit::Matrix ai4x4MatrixToFWMatrix(aiMatrix4x4* m);
    aiVector3D crossProduct(aiVector3D a, aiVector3D b);
    float2 aiVector2DToFloat2(const aiVector2D& v);
    float2 aiVector3DToFloat2(const aiVector3D& v);
    float3 aiVector3DToFloat3(const aiVector3D& v);
    float4 aiVector3DToFloat4(const aiVector3D& v, float w);
    float4 aiQuaternionToFloat4(const aiQuaternion& v);
    float4 aiColor3DToFloat4(const aiColor3D& c);
    float4 aiMatkey4ToFloat4(const aiMaterial*const & mat, const char* key, int a1, int a2);
    float aiMatkeyToFloat(const aiMaterial*const & mat, const char* key, int a1, int a2);
}

DEFINE_EXCEPTION(MeshLoaderException, 9001, "Error while parsing file via ASSIMP");

using namespace Grafkit;
using namespace GkDemo;
using namespace FWdebugExceptions;

#if 0

IResource* IMeshLoader::NewResource() { return new Resource<Mesh>(); }

const aiScene* IMeshLoader::LoadAiScene(IResourceManager* const& resman, Assimp::Importer * const & importer, const char * hint) const
{
    const IAssetRef data = GetSourceAsset(resman);
    const unsigned int aiFlags = aiProcess_ConvertToLeftHanded | aiProcess_CalcTangentSpace;
    const void* dataPtr = data->GetData();
    const size_t length = data->GetSize();
    const aiScene* scene = importer->ReadFileFromMemory(dataPtr, length, aiFlags, hint);

    if (!scene)
        THROW_EX_DETAILS(MeshLoaderException, importer->GetErrorString());

    return scene;
}

void IMeshLoader::LoadMeshes(std::list<MeshRef>& meshes, const aiScene*& sourceScene)
{
    meshes.clear();

    if (!sourceScene->HasMeshes())
        return;

    LOGGER(Log::Logger().Trace("Meshes"));

    for (size_t i = 0; i < sourceScene->mNumMeshes; i++)
    {
        // -- meshes
        MeshRef mesh = new Mesh();
        const aiMesh* srcMesh = sourceScene->mMeshes[i];

        const char* meshName = srcMesh->mName.C_Str();
        LOGGER(Log::Logger().Trace("- Mesh #%d %s", i, meshName));
        //model->SetName(meshName);

        auto vertexCount = srcMesh->mNumVertices;

        float4* vertices = new float4[vertexCount];

        // -- texcooord && normals

        float2* texuvs = new float2[vertexCount];
        float4* normals = new float4[vertexCount];

        for (size_t k = 0; k < vertexCount; k++)
        {
            vertices[k] = aiVector3DToFloat4(srcMesh->mVertices[k], 1.);
            normals[k] = aiVector3DToFloat4(srcMesh->mNormals[k], 0.);

            if (srcMesh->mTextureCoords && srcMesh->mTextureCoords[0])
            {
                const aiVector3D * const& t = srcMesh->mTextureCoords[0];
                if (t)
                    texuvs[k] = aiVector3DToFloat2(t[k]);
                else
                    texuvs[k] = float2(0, 0);
            }
        }

        mesh->AddPointer("POSITION", vertexCount * sizeof(*vertices), &vertices[0]);
        mesh->AddPointer("NORMAL", vertexCount * sizeof(*normals), &normals[0]);

        mesh->AddPointer("TEXCOORD0", vertexCount * sizeof(*texuvs), texuvs);

        // these were copied 
        delete[] texuvs;
        delete[] normals;
        delete[] vertices;

        // -- faces

        if (srcMesh->HasFaces())
        {
            std::vector<uint32_t> indices;
            indices.reserve(srcMesh->mNumFaces * 3 + 1);

            for (size_t j = 0; j < srcMesh->mNumFaces; j++)
            {
                const aiFace &currFace = srcMesh->mFaces[j];
                for (size_t k = 0; k < currFace.mNumIndices; k++) { indices.push_back(currFace.mIndices[k]); }
            }

            mesh->SetIndices(vertexCount, indices.size(), &indices[0]);
        }

        // --- 

        meshes.push_back(mesh);
    }
}

void MeshOBJLoader::Load(IResourceManager* const& resman, IResource* source)
{
    Ref<Resource<Mesh>> dstMesh = dynamic_cast<Resource<Mesh>*>(source);
    assert(dstMesh);

    MeshRef mesh;

    if (!m_persistName.empty())
    {
        try
        {
            IAssetFactory* assetFactory = resman->GetAssetFactory();
            IAssetRef resource = assetFactory->Get(m_persistName.c_str());
            ArchiveAsset archiveAsset(resource);
            mesh = dynamic_cast<Mesh*>(Mesh::Load(archiveAsset));
        }
        catch (std::exception &e)
        {
            LOGGER(Log::Logger().Warn("Cannot load cache %s", m_persistName.c_str()));
            mesh = nullptr;
        }
    }

    // --- 

    if (!mesh)
    {
        Assimp::Importer importer;
        const aiScene* scene = LoadAiScene(resman, &importer, "obj");
        std::list<MeshRef> meshes;
        LoadMeshes(meshes, scene);

        if (meshes.empty())
            THROW_EX_DETAILS(MeshLoaderException, "No meshes loaded");
        if (meshes.size() != 1)
            THROW_EX_DETAILS(MeshLoaderException, "Source contains multiple meshes");

        mesh = meshes.back();

        if (!mesh)
            THROW_EX_DETAILS(MeshLoaderException, "Invlaid mesh was loaded");
    }

    // --- 

    mesh->SetUuid(m_uuid);
    mesh->SetName(m_name);

    dstMesh->AssingnRef(mesh);
}

// TODO -> Library

namespace
{
    Matrix ai4x4MatrixToFWMatrix(aiMatrix4x4* m)
    {
        if (!m)
            THROW_EX(NullPointerException);

        return Matrix(
            m->a1, m->b1, m->c1, m->d1,
            m->a2, m->b2, m->c2, m->d2,
            m->a3, m->b3, m->c3, m->d3,
            m->a4, m->b4, m->c4, m->d4
        );
    }

    aiVector3D crossProduct(aiVector3D a, aiVector3D b)
    {
        return aiVector3D(
            (a.y * b.z - a.z * b.y),
            (a.z * b.x - a.x * b.z),
            (a.x * b.y - a.y * b.x)
        );
    }

    float2 aiVector2DToFloat2(const aiVector2D& v) { return float2(v.x, v.y); }

    float2 aiVector3DToFloat2(const aiVector3D& v) { return float2(v.x, v.y); }

    float3 aiVector3DToFloat3(const  aiVector3D& v) { return float3(v.x, v.y, v.z); }

    float4 aiVector3DToFloat4(const aiVector3D& v, float w) { return float4(v.x, v.y, v.z, w); }

    float4 aiQuaternionToFloat4(const aiQuaternion& v) { return float4(v.x, v.y, v.z, v.w); }

    float4 aiColor3DToFloat4(const aiColor3D& c) { return float4(c.r, c.g, c.b, 1.); }

    float4 aiMatkey4ToFloat4(const aiMaterial*const & m_mat, const char* key, int a1, int a2)
    {
        aiColor4D ac;
        m_mat->Get(key, a1, a2, ac);
        return float4(ac.r, ac.g, ac.g, ac.a);
    }

    float aiMatkeyToFloat(const aiMaterial*const & m_mat, const char* key, int a1, int a2)
    {
        float f = 0.;
        m_mat->Get(key, a1, a2, f);
        return 0.0f;
    }
}

#endif
