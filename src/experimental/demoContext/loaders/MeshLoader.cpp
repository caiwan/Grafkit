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

using namespace Grafkit;
using namespace GkDemo;

// TODO -> Library
namespace
{
    Matrix ai4x4MatrixToFWMatrix(aiMatrix4x4* m);
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

void MeshLoader::Load(IResourceManager* const& resman, IResource* source) {
    // if no cache exists || cached file is old
    if (true)
    {
        Assimp::Importer importer;
        StreamRef data = resman->GetAssetFactory()->Get(m_params.filename);
        const unsigned int aiFlags = aiProcess_ConvertToLeftHanded | aiProcess_CalcTangentSpace;
        std::unique_ptr<uint8_t> dataPtr;
        size_t length = 0;
        data->ReadAll(length, dataPtr);
        const aiScene* scene = importer.ReadFileFromMemory(dataPtr.get(), length, aiFlags, m_params.typeHint.c_str());

        if (!scene)
            throw std::runtime_error(std::string("Could not load mesh: ") + importer.GetErrorString());

        std::vector<MeshRef> meshes;
        LoadMeshes(meshes, scene);

        // lolz
        auto resource = SafeGetResource(source);
        meshes.front()->SetName(m_name);
        meshes.front()->SetUuid(m_uuid);
        resource->AssingnRef(meshes.front());
    }
}

void MeshLoader::Initialize(Renderer& render, IResourceManager* const& resman, IResource* source) {
    // ... 
}


void MeshLoader::LoadMeshes(std::vector<MeshRef>& meshes, const aiScene*& sourceScene)
{
    meshes.clear();

    if (!sourceScene->HasMeshes())
        throw std::runtime_error("File has no meshes");

    LOGGER(Log::Logger().Trace("Meshes"));

    for (size_t i = 0; i < sourceScene->mNumMeshes; i++)
    {
        // -- meshes
        MeshRef mesh = new Mesh();
        const aiMesh* srcMesh = sourceScene->mMeshes[i];

        const char* meshName = srcMesh->mName.C_Str();
        LOGGER(Log::Logger().Trace("- Mesh #%d %s", i, meshName));
        //model->SetName(meshName);

        const auto vertexCount = srcMesh->mNumVertices;

        float4* const vertices = new float4[vertexCount];
        float2* const texuvs = new float2[vertexCount];
        float4* const normals = new float4[vertexCount];

        std::transform(srcMesh->mVertices, srcMesh->mVertices + vertexCount, vertices, [](auto &elem) {return aiVector3DToFloat4(elem, 1.); });
        std::transform(srcMesh->mNormals, srcMesh->mNormals + vertexCount, normals, [](auto &elem) {return aiVector3DToFloat4(elem, 0.); });

        if (srcMesh->mTextureCoords && srcMesh->mTextureCoords[0])
            std::transform(srcMesh->mTextureCoords[0], srcMesh->mTextureCoords[0] + vertexCount, texuvs, [](auto &elem) {return aiVector3DToFloat2(elem); });
        else
            std::fill_n(texuvs, vertexCount, float2(0,0));
    
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

            std::for_each_n(srcMesh->mFaces, srcMesh->mNumFaces, [&indices](auto& face)
            {
                std::copy_n(face.mIndices, face.mNumIndices, std::back_inserter(indices));
            });

            mesh->SetIndices(vertexCount, indices.size(), &indices[0]);
        }
        else
            throw std::runtime_error("Mesh has no faces");

        // --- 

        meshes.push_back(mesh);
    }
}

// TODO -> Library
namespace
{
    Matrix ai4x4MatrixToFWMatrix(aiMatrix4x4* m)
    {
        assert(m);
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
