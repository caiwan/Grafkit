#include "stdafx.h"
#include "cube.h" 

using namespace Grafkit;

static const float _triangle[]= { 
	0.0f,  1.0f, -0.1f,
	1.0f, -1.0f, -0.1f,
	-1.0f, -1.0f,-0.1f,
	
	-1.0f, -1.0f, 0.1f,
	1.0f, -1.0f,  0.1f,
	0.0f,  1.0f,  0.1f,
 };

static const unsigned short _triangleIndices[] = {
	0,1,2,
	3,4,5,
 };

const float * const GrafkitData::triangle = _triangle;
const unsigned short * const  GrafkitData::triangleIndices = _triangleIndices;
const unsigned short GrafkitData::triangleIndicesLen = 3;


static const float _quad_vertices[] = {
	-1,-1, 0, 0,
	1,-1, 0, 0,
	1, 1, 0, 0,
	-1, 1, 0, 0,
};

static const float _quad_vertices2[] = {
	0, 0, 0, 0,
	1, 0, 0, 0,
	1, 1, 0, 0,
	0, 1, 0, 0,
};

static const float _quad_texcoord[] = {
	0, 1, 
	1, 1, 
	1, 0, 
	0, 0
};

static const int _quad_mesh[] = {
	0,3,2,
	2,1,0
};

const float * const GrafkitData::quad = _quad_vertices;
const float * const GrafkitData::quad2 = _quad_vertices2;
const float * const GrafkitData::quad_texcoord = _quad_texcoord;
const int * const GrafkitData::quadIndices = _quad_mesh;

// --- Cube 

// Front Face
// Back Face
// Top Face
// Bottom Face
// Right face
// Left Face


static const float _cube_normals [] = {
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,

	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,

	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,

	0.0f, -1.0f, 0.0, 0.0f,
	0.0f, -1.0f, 0.0, 0.0f,
	0.0f, -1.0f, 0.0, 0.0f,
	0.0f, -1.0f, 0.0, 0.0f,

	1.0f, 0.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f, 0.0f,

	1.0f, 0.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f, 0.0f,
};

static const float _cube_texcoords[] = {
	0.0f, 0.0f,
	1.0f, 0.0f,
	1.0f, 1.0f,
	0.0f, 1.0f,
			
	1.0f, 0.0f,
	1.0f, 1.0f,
	0.0f, 1.0f,
	0.0f, 0.0f,
			
	0.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 0.0f,
	1.0f, 1.0f,
			
	1.0f, 1.0f,
	0.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 0.0f,
			
	1.0f, 0.0f,
	1.0f, 1.0f,
	0.0f, 1.0f,
	0.0f, 0.0f,
			
	0.0f, 0.0f,
	1.0f, 0.0f,
	1.0f, 1.0f,
	0.0f, 1.0f,
};



static const float _cube_vertices []= {
	-1.0f, -1.0f, 1.0f ,0.0f,    // Point 1 (Front)
	1.0f, -1.0f, 1.0f  ,0.0f,    // Point 2 (Front)
	1.0f, 1.0f, 1.0f   ,0.0f,    // Point 3 (Front)
	-1.0f, 1.0f, 1.0f  ,0.0f,    // Point 4 (Front)

	-1.0f, -1.0f, -1.0f,0.0f,     // Point 1 (Back)
	-1.0f, 1.0f, -1.0f ,0.0f,    // Point 2 (Back)
	1.0f, 1.0f, -1.0f  ,0.0f,   // Point 3 (Back)
	1.0f, -1.0f, -1.0f ,0.0f,    // Point 4 (Back)

	-1.0f, 1.0f, -1.0f ,0.0f,    // Point 1 (Top)
	-1.0f, 1.0f, 1.0f  ,0.0f,   // Point 2 (Top)
	1.0f, 1.0f, 1.0f   ,0.0f,   // Point 3 (Top)
	1.0f, 1.0f, -1.0f  ,0.0f,   // Point 4 (Top)

	-1.0f, -1.0f, -1.0f,0.0f,     // Point 1 (Bottom)
	1.0f, -1.0f, -1.0f ,0.0f,    // Point 2 (Bottom)
	1.0f, -1.0f, 1.0f  ,0.0f,   // Point 3 (Bottom)
	-1.0f, -1.0f, 1.0f ,0.0f,    // Point 4 (Bottom)

	1.0f, -1.0f, -1.0f ,0.0f,    // Point 1 (Right)
	1.0f, 1.0f, -1.0f  ,0.0f,   // Point 2 (Right)
	1.0f, 1.0f, 1.0f   ,0.0f,   // Point 3 (Right)
	1.0f, -1.0f, 1.0f  ,0.0f,   // Point 4 (Right)

	-1.0f, -1.0f, -1.0f,0.0f,     // Point 1 (Left)
	-1.0f, -1.0f, 1.0f ,0.0f,    // Point 2 (Left)
	-1.0f, 1.0f, 1.0f  ,0.0f,   // Point 3 (Left)
	-1.0f, 1.0f, -1.0f ,0.0f,    // Point 4 (Left)
};

static const int _cube_indices[] = {
	0,   1,  2,     2,  3,  0,		// 0
	4,   5,  6,     6,  7,  4,		// 1
	8,   9, 10,    10, 11,  8,		// 2
	12, 13, 14,    14, 15, 12,		// 3
	16, 17, 18,    18, 19, 16,      // 4
	20, 21, 22,    22, 23, 20       // 5
};

/**/
const float * const GrafkitData::cubeVertices = _cube_vertices;
const float * const GrafkitData::cubeNormals = _cube_normals;
///@todo add tangents also
const float * const GrafkitData::cubeTextureUVs = _cube_texcoords;
const   int * const GrafkitData::cubeIndices = _cube_indices;

const unsigned int GrafkitData::cubeVertexCount = 6 * 4;
const unsigned int GrafkitData::cubeIndicesCount = 6 * 6;

const size_t GrafkitData::cubeVertexSize     = sizeof(_cube_vertices);
const size_t GrafkitData::cubeNormalSize 	 = sizeof(_cube_normals);
const size_t GrafkitData::cubeTextureUVsSize = sizeof(_cube_texcoords);
const size_t GrafkitData::cubeIndicesSize	 = sizeof(_cube_indices);

Grafkit::MeshRef GrafkitData::CreateCube()
{
	MeshRef mesh = new Mesh();
	mesh->AddPointer("POSITION", GrafkitData::cubeVertexSize, GrafkitData::cubeVertices);
	mesh->AddPointer("TEXCOORD", GrafkitData::cubeVertexSize, GrafkitData::cubeTextureUVs);
	mesh->AddPointer("NORMAL", GrafkitData::cubeVertexSize, GrafkitData::cubeNormals);
	mesh->SetIndices(GrafkitData::cubeVertexCount, GrafkitData::cubeIndicesCount, GrafkitData::cubeIndices);
	return mesh;
}
