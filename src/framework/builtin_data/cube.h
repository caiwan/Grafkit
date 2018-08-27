#pragma once
#include "common.h"

/**
 *  @file
 *  @brief static geometry data
 */

namespace GrafkitData
{
    extern const float* const triangle; ///< Vertices of a single triangle
    extern const unsigned short* const triangleIndices; ///< Indices of a single triangle
    extern const unsigned short triangleIndicesLen; ///< Number of indices of a signle triangle

    extern const float* const quad; ///< Vertices of a single quad [-1..1]
    extern const float* const quad2; ///< Vertices of a single quad [0..1]
    extern const float* const quad_texcoord; ///< Texture coordinates of a single quad; 2 component
    extern const uint32_t* const quadIndices; ///< Indices of a single quad

    extern const float* const cubeVertices; ///< Vertices of a cube
    extern const float* const cubeNormals; ///< Normals of a cube
    extern const float* const cubeTextureUVs; ///< Textire coordinates of a cube
    extern const uint32_t* const cubeIndices; ///< Indices of a cube

    extern const unsigned int cubeVertexCount; ///< Number of vertices of a cube
    extern const unsigned int cubeIndicesCount; ///< Number of indices of a cube

    extern const size_t cubeVertexSize;
    extern const size_t cubeNormalSize;
    extern const size_t cubeTextureUVsSize;
    extern const size_t cubeIndicesSize;

    extern Grafkit::MeshRef CreateQuad();

    extern Grafkit::MeshRef CreateCube();
    extern Grafkit::MeshRef CreateCubes(size_t count);
}
