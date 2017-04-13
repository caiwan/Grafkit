#include "stdafx.h"

#include "objloader.inc"

#include "cube.h" 

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

// Cube 

/**/

const float cube_normals [] = {
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

const float cube_texcoords[] = {
	0.0f, 0.0f, 0.0f, 0.0f,
	1.0f, 0.0f,	0.0f, 0.0f,
	1.0f, 1.0f,	0.0f, 0.0f,
	0.0f, 1.0f,	0.0f, 0.0f,
				
	1.0f, 0.0f,	0.0f, 0.0f,
	1.0f, 1.0f,	0.0f, 0.0f,
	0.0f, 1.0f,	0.0f, 0.0f,
	0.0f, 0.0f,	0.0f, 0.0f,
				
	0.0f, 1.0f,	0.0f, 0.0f,
	0.0f, 0.0f,	0.0f, 0.0f,
	1.0f, 0.0f,	0.0f, 0.0f,
	1.0f, 1.0f,	0.0f, 0.0f,
				
	1.0f, 1.0f,	0.0f, 0.0f,
	0.0f, 1.0f,	0.0f, 0.0f,
	0.0f, 0.0f,	0.0f, 0.0f,
	1.0f, 0.0f,	0.0f, 0.0f,
				
	1.0f, 0.0f,	0.0f, 0.0f,
	1.0f, 1.0f,	0.0f, 0.0f,
	0.0f, 1.0f,	0.0f, 0.0f,
	0.0f, 0.0f,	0.0f, 0.0f,
				
	0.0f, 0.0f,	0.0f, 0.0f,
	1.0f, 0.0f,	0.0f, 0.0f,
	1.0f, 1.0f,	0.0f, 0.0f,
	0.0f, 1.0f,	0.0f, 0.0f,

};

// Front Face
// Back Face
// Top Face
// Bottom Face
// Right face
// Left Face

const float cube_vertices = {
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

/**/
const float * const GrafkitData::cubeVertices = NULL;
const float * const GrafkitData::cubeNormals = NULL;
///@todo add tangents also
const float * const GrafkitData::cubeTextureUVs = NULL;
const   int * const GrafkitData::cubeIndices = NULL;

static const unsigned int _cube_vertex_length = 0;
static const unsigned int _cube_indices_length = 0;

const unsigned int GrafkitData::cubeVertexLength = 0;
const unsigned int GrafkitData::cubeIndicesLength = 0;
