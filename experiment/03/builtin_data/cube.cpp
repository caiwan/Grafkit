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

const float * const FWBuiltInData::triangle = _triangle;
const unsigned short * const  FWBuiltInData::triangleIndices = _triangleIndices;
const unsigned short FWBuiltInData::triangleIndicesLen = 3;


static const float _quad_vertices[] = {
	-1,-1, 0,
	1,-1, 0,
	1, 1, 0,
	-1, 1, 0
};

static const float _quad_vertices2[] = {
	0, 0, 0,
	1, 0, 0,
	1, 1, 0,
	0, 1, 0
};

static const float _quad_texcoord[] = {
	0, 1, 
	1, 1, 
	1, 0, 
	0, 0
};

static const unsigned short _quad_mesh[] = {
	0,1,2, 
	2,3,0
};

const float * const FWBuiltInData::quad = _quad_vertices;
const float * const FWBuiltInData::quad2 = _quad_vertices2;
const float * const FWBuiltInData::quad_texcoord = _quad_texcoord;
const unsigned short * const FWBuiltInData::quadIndices = _quad_mesh;
const unsigned short FWBuiltInData::quadIndicesLen = 4;

#if 1
// vertex
static const float _cube_vertices[] = {	
		//0
		-1.0f, -1.0f,  1.0f,	//0
		 1.0f, -1.0f,  1.0f,	//1
		 1.0f,  1.0f,  1.0f,	//2
		-1.0f,  1.0f,  1.0f,	//3
		//1
		-1.0f, -1.0f, -1.0f,	//4
		-1.0f,  1.0f, -1.0f,	//5
		 1.0f,  1.0f, -1.0f,	//6
		 1.0f, -1.0f, -1.0f,	//7
		//2
		-1.0f,  1.0f, -1.0f,	//8
		-1.0f,  1.0f,  1.0f,	//9
		 1.0f,  1.0f,  1.0f,	//10
		 1.0f,  1.0f, -1.0f,	//11
		//3
		-1.0f, -1.0f, -1.0f,	//12
		 1.0f, -1.0f, -1.0f,	//13
		 1.0f, -1.0f,  1.0f,	//14
		-1.0f, -1.0f,  1.0f,	//15
		//4
		 1.0f, -1.0f, -1.0f,	//16
		 1.0f,  1.0f, -1.0f,	//17
		 1.0f,  1.0f,  1.0f,	//18
		 1.0f, -1.0f,  1.0f,	//19
		//5
		-1.0f, -1.0f, -1.0f,	//20
		-1.0f, -1.0f,  1.0f,	//21
		-1.0f,  1.0f,  1.0f,	//22
		-1.0f,  1.0f, -1.0f		//23
};

// normals
static const float _cube_normals[] = {
		
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		
		0.0f, 0.0f,-1.0f,
		0.0f, 0.0f,-1.0f,
		0.0f, 0.0f,-1.0f,
		0.0f, 0.0f,-1.0f,
		
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		
		0.0f,-1.0f, 0.0f,
		0.0f,-1.0f, 0.0f,
		0.0f,-1.0f, 0.0f,
		0.0f,-1.0f, 0.0f,
		
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f
};

// texture
static const float _cube_texture_uvs[] = {		
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
		0.0f, 1.0f
};

#if 1

static const int _cube_indices[] = {
	0,   1,  2,     2,  3,  0,		// 0
	4,   5,  6,     6,  7,  4,		// 1
	8,   9, 10,    10, 11,  8,		// 2
	12, 13, 14,    14, 15, 12,		// 3
	16, 17, 18,    18, 19, 16,      // 4
	20, 21, 22,    22, 23, 20       // 5
};

#else

static const unsigned short _cube_indices[] = {
	2,   1,  0,     0,  3,  2,		// 0
	6,   5,  4,     4,  7,  6,		// 1
	10,  9, 8,     8,  11, 10,		// 2
	14, 13, 12,    12, 15, 14,		// 3
	18, 17, 16,    16, 19, 18,      // 4
	22, 21, 20,    20, 23, 22       // 5
};

#endif
#else 
static const float _cube_vertices[] = {	
	1.000000, 1.000000, -1.000000,
	1.000000, -1.000000, -1.000000,
	-1.000000, -1.000000, -1.000000,
	-1.000000, 1.000000, 1.000000,
	-1.000000, -1.000000, 1.000000,
	1.000000, -1.000000, 1.000000,
	1.000000, 1.000000, 1.000000,
	1.000000, -1.000000, 1.000000,
	1.000000, -1.000000, -1.000000,
	1.000000, -1.000000, 1.000000,
	-1.000000, -1.000000, 1.000000,
	-1.000000, -1.000000, -1.000000,
	-1.000000, -1.000000, -1.000000,
	-1.000000, -1.000000, 1.000000,
	-1.000000, 1.000000, 1.000000,
	1.000000, 1.000000, 1.000000,
	1.000000, 1.000000, -1.000000,
	-1.000000, 1.000000, -1.000000,
	-1.000000, 1.000000, -1.000000,
	1.000000, 1.000000, 1.000000,
	1.000000, 1.000000, -1.000000,
	1.000000, -1.000000, -1.000000,
	-1.000000, 1.000000, -1.000000,
	-1.000000, 1.000000, 1.000000,
  };

static const float _cube_normals[] = {
	0.000000, 0.000000, -1.000000,
	0.000000, 0.000000, -1.000000,
	0.000000, 0.000000, -1.000000,
	0.000000, 0.000000, 1.000000,
	0.000000, 0.000000, 1.000000,
	0.000000, 0.000000, 1.000000,
	1.000000, 0.000000, 0.000000,
	1.000000, 0.000000, 0.000000,
	1.000000, 0.000000, 0.000000,
	0.000000, -1.000000, 0.000000,
	0.000000, -1.000000, 0.000000,
	0.000000, -1.000000, 0.000000,
	-1.000000, 0.000000, 0.000000,
	-1.000000, 0.000000, 0.000000,
	-1.000000, 0.000000, 0.000000,
	0.000000, 1.000000, 0.000000,
	0.000000, 1.000000, 0.000000,
	0.000000, 1.000000, 0.000000,
	0.000000, 0.000000, -1.000000,
	0.000000, 0.000000, 1.000000,
	1.000000, 0.000000, 0.000000,
	0.000000, -1.000000, 0.000000,
	-1.000000, 0.000000, 0.000000,
	0.000000, 1.000000, 0.000000,
  };

static const float _cube_texture_uvs[] = {		
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
		0.0f, 1.0f
};

static const unsigned short _cube_indices[] = {
23,15,17,
15,16,17,
22,12,14,
12,13,14,
21,9,11,9,
10,11,20,
6,8,6,7,
8,19,3,
5,3,4,5,
18,0,2,
0,1,2,  };
#endif


const float * const FWBuiltInData::cubeVertices = _cube_vertices;
const float * const FWBuiltInData::cubeNormals = _cube_normals;
const float * const FWBuiltInData::cubeTextureUVs =_cube_texture_uvs;
const   int * const FWBuiltInData::cubeIndices = _cube_indices;

static const unsigned int _cube_vertex_length = sizeof(_cube_vertices)/sizeof(*_cube_vertices) / 3;
static const unsigned int _cube_indices_length = sizeof(_cube_indices)/sizeof(*_cube_indices);

const unsigned int FWBuiltInData::cubeVertexLength = _cube_vertex_length;
const unsigned int FWBuiltInData::cubeIndicesLength = _cube_indices_length;
