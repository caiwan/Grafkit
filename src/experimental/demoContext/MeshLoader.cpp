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

#include "utils/AssetFactory.h"
#include "utils/resource.h"
#include "utils/ResourceManager.h"

#include "utils/persistence/archive.h"
#include "utils/persistence/persistence.h"

#include "math/matrix.h"
#include "render/mesh.h"

#define MAX_TEXCOORD_COUNT 2

using namespace Grafkit;
using namespace GkDemo;
using namespace FWdebugExceptions;

// TODO -> Library

namespace
{
    Matrix ai4x4MatrixToFWMatrix(aiMatrix4x4* m);
    aiVector3D crossProduct(aiVector3D a, aiVector3D b);
    float2 aiVector2DToFloat2(aiVector2D& v);
    float2 aiVector3DToFloat2(aiVector3D& v);
    float3 aiVector3DToFloat3(aiVector3D& v);
    float4 aiVector3DToFloat4(aiVector3D& v, float w);
    float4 aiQuaternionToFloat4(aiQuaternion& v);
    float4 aiColor3DToFloat4(aiColor3D& c);
    float4 aiMatkey4ToFloat4(aiMaterial*& mat, const char* key, int a1, int a2);
    float aiMatkeyToFloat(aiMaterial*& mat, const char* key, int a1, int a2);
}

IResource* IMeshLoader::NewResource() { return new Resource<Mesh>(); }

const aiScene* IMeshLoader::LoadAiScene(IResourceManager* const& resman) const
{
    Assimp::Importer importer;
    const IAssetRef data = GetSourceAsset(resman);
    const unsigned aiFlags = aiProcess_ConvertToLeftHanded | aiProcess_CalcTangentSpace;
    //const aiScene* scene = importer.ReadFileFromMemory(data->GetData(), data->GetSize(), aiFlags);
    const aiScene* scene = nullptr;

    if (!scene)
        THROW_EX_DETAILS(MeshLoaderException, importer.GetErrorString());

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
        //ModelRef model = new Model(new Mesh());
        MeshRef mesh = new Mesh();
        aiMesh* srcMesh = sourceScene->mMeshes[i];

        const char* meshName = srcMesh->mName.C_Str();
        LOGGER(Log::Logger().Trace("- Mesh #%d %s", i, meshName));
        //model->SetName(meshName);

        float4* vertices = new float4[srcMesh->mNumVertices];

        // -- texcooord && normals

        float2** texuvs = new float2*[MAX_TEXCOORD_COUNT];
        for (size_t j = 0; j < MAX_TEXCOORD_COUNT; j++) { texuvs[j] = new float2[srcMesh->mNumVertices]; }
        float4* normals = new float4[srcMesh->mNumVertices];

        for (size_t k = 0; k < srcMesh->mNumVertices; k++)
        {
            vertices[k] = aiVector3DToFloat4(srcMesh->mVertices[k], 1.);
            normals[k] = aiVector3DToFloat4(srcMesh->mNormals[k], 0.);

            if (srcMesh->mTextureCoords && srcMesh->mTextureCoords[0])
            {
                for (size_t j = 0; j < MAX_TEXCOORD_COUNT; j++)
                {
                    aiVector3D* t = srcMesh->mTextureCoords[j];
                    if (t)
                        texuvs[j][k] = aiVector3DToFloat2(t[k]);
                    else
                        texuvs[j][k] = float2(0, 0);
                }
            }

            mesh->AddPointer("POSITION", srcMesh->mNumVertices * sizeof(*vertices), &vertices[0]);
            mesh->AddPointer("NORMAL", srcMesh->mNumVertices * sizeof(*normals), &normals[0]);

            for (size_t j = 0; j < MAX_TEXCOORD_COUNT; j++)
            {
                std::string buf = "TEXCOORD" + std::to_string(i);
                mesh->AddPointer(buf, srcMesh->mNumVertices * sizeof(**texuvs), &texuvs[j]);
            }
        }

        // these will be copied 
        delete[] texuvs;
        delete[] normals;

        // -- faces

        if (srcMesh->HasFaces())
        {
            std::vector<uint32_t> indices;

            for (size_t j = 0; j < srcMesh->mNumFaces; j++)
            {
                aiFace* curr_face = &srcMesh->mFaces[j];
                for (size_t k = 0; k < curr_face->mNumIndices; k++) { indices.push_back(curr_face->mIndices[k]); }
            }

            mesh->SetIndices(srcMesh->mNumVertices, indices.size(), &indices[0]);
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
        const aiScene* scene = LoadAiScene(resman);
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

    float2 aiVector2DToFloat2(aiVector2D& v) { return float2(v.x, v.y); }

    float2 aiVector3DToFloat2(aiVector3D& v) { return float2(v.x, v.y); }

    float3 aiVector3DToFloat3(aiVector3D& v) { return float3(v.x, v.y, v.z); }

    float4 aiVector3DToFloat4(aiVector3D& v, float w) { return float4(v.x, v.y, v.z, w); }

    float4 aiQuaternionToFloat4(aiQuaternion& v) { return float4(v.x, v.y, v.z, v.w); }

    float4 aiColor3DToFloat4(aiColor3D& c) { return float4(c.r, c.g, c.b, 1.); }

    float4 aiMatkey4ToFloat4(aiMaterial*& mat, const char* key, int a1, int a2)
    {
        aiColor4D ac;
        mat->Get(key, a1, a2, ac);
        return float4(ac.r, ac.g, ac.g, ac.a);
    }

    float aiMatkeyToFloat(aiMaterial*& mat, const char* key, int a1, int a2)
    {
        float f = 0.;
        mat->Get(key, a1, a2, f);
        return 0.0f;
    }
}
