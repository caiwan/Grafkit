#include "stdafx.h"

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

const float * const GrafkitData::cubeVertices = NULL;
const float * const GrafkitData::cubeNormals = NULL;
///@todo add tangents also
const float * const GrafkitData::cubeTextureUVs = NULL;
const   int * const GrafkitData::cubeIndices = NULL;

static const unsigned int _cube_vertex_length = 0;
static const unsigned int _cube_indices_length = 0;

const unsigned int GrafkitData::cubeVertexLength = 0;
const unsigned int GrafkitData::cubeIndicesLength = 0;

static const float _cube_vertex_data[512];
static const int _cube_index_data[256];

//

const char * const cubeOBJ = ""
//"newmtl Material\n"
//"Ns 96.078431\n"
//"Ka 1.000000 1.000000 1.000000\n"
//"Kd 0.640000 0.640000 0.640000\n"
//"Ks 0.500000 0.500000 0.500000\n"
//"Ke 0.000000 0.000000 0.000000\n"
//"Ni 1.000000\n"
//"d 1.000000\n"
//"illum 2\n"
//"\n"
"o Cube\n"
"\n"
"v 1.000000 - 1.000000 - 1.000000\n"
"v 1.000000 - 1.000000 1.000000\n"
"v - 1.000000 - 1.000000 1.000000\n"
"v - 1.000000 - 1.000000 - 1.000000\n"
"v 1.000000 1.000000 - 1.000000\n"
"v 1.000000 1.000000 1.000001\n"
"v - 1.000000 1.000000 1.000000\n"
"v - 1.000000 1.000000 - 1.000000\n"
"\n"
"vn 0.0000 - 1.0000 0.0000\n"
"vn 0.0000 1.0000 0.0000\n"
"vn 1.0000 0.0000 0.0000\n"
"vn - 0.0000 - 0.0000 1.0000\n"
"vn - 1.0000 - 0.0000 - 0.0000\n"
"vn 0.0000 0.0000 - 1.0000\n"
"\n"
//"usemtl Material\n"
"s off\n"
"f 1//1 2//1 3//1 4//1\n"
"f 5//2 8//2 7//2 6//2\n"
"f 1//3 5//3 6//3 2//3\n"
"f 2//4 6//4 7//4 3//4\n"
"f 3//5 7//5 8//5 4//5\n"
"f 5//6 1//6 4//6 8//6\n";

/* QnD obj parser*/
#include <string>
#include <iostream>
#include <sstream>
#include <vector>

namespace {

	typedef std::vector<float> floatlist;
	typedef std::vector<unsigned int> uintlist;

	struct VertexData { floatlist vertices, normals, texcoords; };
	struct FaceData { uintlist vertices, normals, texcoords; };

	struct Mesh { VertexData v; uintlist indices; };

	static void ParseFloats(const std::string &in, floatlist &out) {
		std::stringstream ss(in);
		std::string tok;
		do {
			bool neg = false;
			std::getline(ss, tok, ' ');
			if (tok.empty())
				continue;
			if (tok[0] == '-') {
				neg = true;
				std::getline(ss, tok, ' ');
			}
			float f = std::stof(tok) * (neg ? -1. : 1.);
			out.push_back(f);
		} while (ss.eof());
	}

	static void ParseFace(const std::string &in, uintlist &out) {
		// ... 
	}

	static void ParseFaces(const std::string &in, FaceData &out) {
		// ... 
	}
}

void GrafkitData::CalcCube() {
	std::stringstream in(cubeOBJ);
	std::string line;

	VertexData v;
	FaceData f;

	do {
		std::getline(in, line);
		if (!line.empty()) {
			size_t t = line.find(' ');
			std::string command = line.substr(0, t), value = line.substr(t, line.length());
			
			if (command.compare("v") == 0) ParseFloats(value, v.vertices);
			else if (command.compare("vn") == 0) ParseFloats(value, v.normals);
			else if (command.compare("f") == 0) ParseFaces(value, f);
		}
	} while (!in.eof());
}


