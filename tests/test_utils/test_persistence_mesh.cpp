/*
TCs for internal classes such as scene and so on
*/

#define _CRT_SECURE_NO_WARNINGS

#include "stdafx.h"

#include <gtest/gtest.h>

#include "render/mesh.h"

#include "TestArchiver.h"
#include "testClass_persistence.h"

using namespace Grafkit;
using namespace FWdebugExceptions;
using namespace ArchivePersistent;

TEST(Persistent_Mesh, MeshPersistentTest)
{
    TestArchiver ar(64 * 4096, true);

    // given
    size_t vertexCount = 256;
    float * const vertices = new float[vertexCount * 4];
    for (size_t i = 0; i < vertexCount * 4; i++) { vertices[i] = i; }

    size_t indexCount = 512;
    uint32_t  * const indices = new uint32_t[indexCount];
    for (size_t i = 0; i < indexCount; i++) { indices[i] = i; }

    MeshRef mesh = new Mesh();
    mesh->AddPointer("POSITION", vertexCount, vertices);
    mesh->AddPointer("NORMAL", vertexCount, vertices);
    mesh->AddPointer("TEXCOORD", vertexCount, vertices);
    mesh->AddPointer("BINORMAL", vertexCount, vertices);

    mesh->SetIndices(vertexCount, indexCount, indices);

    //when

    mesh->Store(ar);

    //then

    size_t pos = ar.GetCrsr();
    ar.ResetCrsr();
    ar.SetDirection(false);

    MeshRef loadedMesh = dynamic_cast<Mesh*>(Persistent::Load(ar));

    ASSERT_TRUE(loadedMesh.Valid());

    // TODO: Refactro if you see this

    const float * const  loadedVertices = reinterpret_cast<const float*>(loadedMesh->GetPointer("POSITION"));
    for (size_t i = 0; i < vertexCount * 4; i++) { ASSERT_EQ(loadedVertices[i], vertices[i]); }

    const float * const  loadedVertices2 = reinterpret_cast<const float*>(loadedMesh->GetPointer("NORMAL"));
    for (size_t i = 0; i < vertexCount * 4; i++) { ASSERT_EQ(loadedVertices2[i], vertices[i]); }

    const float * const  loadedVertices3 = reinterpret_cast<const float*>(loadedMesh->GetPointer("TEXCOORD"));
    for (size_t i = 0; i < vertexCount * 4; i++) { ASSERT_EQ(loadedVertices3[i], vertices[i]); }

    const float * const  loadedVertices4 = reinterpret_cast<const float*>(loadedMesh->GetPointer("BINORMAL"));
    for (size_t i = 0; i < vertexCount * 4; i++) { ASSERT_EQ(loadedVertices4[i], vertices[i]); }

    ASSERT_EQ(indexCount, mesh->GetIndexCount());

    const uint32_t * loadedIndices = loadedMesh->GetIndices();
    for (size_t i = 0; i < indexCount; i++) { ASSERT_EQ(loadedIndices[i], indices[i]); }

    delete[]vertices;
    delete[]indices;
}
