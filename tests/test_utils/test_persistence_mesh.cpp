/*
TCs for internal classes such as scene and so on
*/

#define _CRT_SECURE_NO_WARNINGS

#include "stdafx.h"

#include "cout.h"

#include <gtest/gtest.h>

#include "render/mesh.h"

#include "TestArchiver.h"
#include "testClass_persistence.h"

using namespace Grafkit;
using namespace FWdebugExceptions;
using namespace ArchivePersistent;

class PersistentMeshTest : public testing::Test
{
protected:
    void SetUp() override {
    }

    void TearDown() override {
    }

    void ValidateMeshPtr(const MeshRef& actual, std::string name, uint32_t vertexCount, uint32_t stride) const
    {
        const float * const vertices = reinterpret_cast<const float*>(actual->GetPointer(name));
        ASSERT_TRUE(vertices);
        for (uint32_t i = 0; i < vertexCount * stride; i++)
        {
            ASSERT_EQ(vertices[i], vertices[i]) << name.c_str() << i;
        }
    }

};

TEST_F(PersistentMeshTest, MeshPersistentTest)
{
    TestArchiver ar(64 * 4096, true);

    // given
    uint32_t vertexCount = 256;
    float * const vertices = new float[vertexCount * 4];
    for (uint32_t i = 0; i < vertexCount * 4; i++) { vertices[i] = rand() / RAND_MAX; }

    uint32_t indexCount = 512;
    uint32_t  * const indices = new uint32_t[indexCount];
    for (uint32_t i = 0; i < indexCount; i++) { indices[i] = rand() % 256; }

    MeshRef mesh = new Mesh();
    mesh->AddPointer("POSITION", vertexCount * 4 * 4, vertices);
    mesh->AddPointer("NORMAL", vertexCount * 4 * 4, vertices);
    mesh->AddPointer("TEXCOORD", vertexCount * 4 * 4, vertices);
    mesh->AddPointer("BINORMAL", vertexCount * 4 * 4, vertices);

    mesh->SetIndices(vertexCount, indexCount, indices);

    //when

    mesh->Store(ar);

    //then
    uint32_t pos = ar.GetCrsr();
    ar.ResetCrsr();
    ar.SetDirection(false);

    MeshRef loadedMesh = dynamic_cast<Mesh*>(Persistent::Load(ar));

    ASSERT_TRUE(loadedMesh.Valid());

    // TODO: Refactro if you see this

    ValidateMeshPtr(loadedMesh, "POSITION", vertexCount, 4);
    ValidateMeshPtr(loadedMesh, "NORMAL", vertexCount, 4);
    ValidateMeshPtr(loadedMesh, "TEXCOORD", vertexCount, 4);
    ValidateMeshPtr(loadedMesh, "BINORMAL", vertexCount, 4);

    ASSERT_EQ(indexCount, mesh->GetIndexCount());

    const uint32_t * loadedIndices = loadedMesh->GetIndices();
    for (uint32_t i = 0; i < indexCount; i++)
    {
        ASSERT_EQ(loadedIndices[i], indices[i]) << i;
    }

    delete[]vertices;
    delete[]indices;
}
