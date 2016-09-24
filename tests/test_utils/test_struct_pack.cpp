#include <gtest/gtest.h>


#include "utils/struct_pack.h"
#include "utils/exceptions.h"

using namespace Grafkit;
using namespace FWdebugExceptions;

// ============================================================

struct test_0_t {
	int m_int;
	float m_float;
};

/// Basic packing/formatting test
TEST(Struct, Pack)
{
	const int test_0_0[] = { 0, 1, 2, 3, 4, 5, 6, 7 };
	const float test_0_1[] = { 0.1, 1.2, 2.3, 3.4, 4.5, 5.6, 6.7, 7.8 };
	
	// ----- 

	StructPack packer;
	int id0 = packer.addField(sizeof(int));
	int id1 = packer.addField(sizeof(float));

	packer.addPointer(id0, test_0_0, 0, sizeof(int));
	packer.addPointer(id1, test_0_1, 0, sizeof(float));

	void *result = packer(7);
	ASSERT_FALSE(result == NULL);

	struct test_0_t* result_data = (struct test_0_t*) result;

	for (size_t i = 0; i < 7; i++) 
	{
		ASSERT_EQ(test_0_0[i], result_data[i].m_int);
		ASSERT_TRUE(result_data[i].m_float - test_0_1[i] < 0.00001);
	}
}

// Test struct with an undefined field
TEST(Struct, PackUndefinedPtr)
{
	const int test_0_0[] = { 0, 1, 2, 3, 4, 5, 6, 7 };
	const float test_0_1[] = { 0.1, 1.2, 2.3, 3.4, 4.5, 5.6, 6.7, 7.8 };

	// ------- 
	
	StructPack packer;
	int id0 = packer.addField(sizeof(int));
	int id1 = packer.addField(sizeof(float));

	packer.addPointer(id0, test_0_0, 0, sizeof(int));

	void *result = packer(7);
	ASSERT_FALSE(result == NULL);

	struct test_0_t* result_data = (struct test_0_t*) result;

	for (size_t i = 0; i < 7; i++)
	{
		ASSERT_EQ(test_0_0[i], result_data[i].m_int);
		ASSERT_DOUBLE_EQ(result_data[i].m_float, 0.0);
	}

	// ----

	StructPack packer2;
	id0 = packer2.addField(sizeof(int));
	id1 = packer2.addField(sizeof(float));

	packer2.addPointer(id1, test_0_1, 0, sizeof(float));

	result = packer2(7);
	ASSERT_FALSE(result == NULL);

	result_data = (struct test_0_t*) result;

	for (size_t i = 0; i < 7; i++)
	{
		ASSERT_EQ(result_data[i].m_int, 0);
		ASSERT_TRUE(result_data[i].m_float - test_0_1[i] < 0.00001);
	}
}

// ============================================================
/// Formatting with larger stride on the input side
TEST(Struct, Stride)
{
	const int test_1_0[] = { 0, 0, 1, 0, 2, 0, 3, 0, 4, 0, 5, 0, 6, 0, 7 };
	const float test_1_1[] = { 0.1, 0, 1.2, 0, 2.3, 0, 3.4, 0, 4.5, 0, 5.6, 0, 6.7, 0, 7.8 };

	// ---- 

	StructPack packer;
	int id0 = packer.addField(sizeof(int));
	int id1 = packer.addField(sizeof(float));

	packer.addPointer(id0, test_1_0, 0, 2 * sizeof(int));
	packer.addPointer(id1, test_1_1, 0, 2 * sizeof(float));

	void *result = packer(7);
	ASSERT_FALSE(result == NULL);

	struct test_0_t* result_data = (struct test_0_t*) result;

	for (size_t i = 0; i < 7; i++)
	{
		ASSERT_EQ(test_1_0[2*i], result_data[i].m_int);
		ASSERT_TRUE(result_data[i].m_float - test_1_1[2*i] < 0.00001);
	}
}

// ============================================================

/// Formatting with larger stride and byte offset on the input side
TEST(Struct, Offset)
{
	const int test_2_0[] = { 0, 1, 0, 2, 0, 3, 0, 4, 0, 5, 0, 6, 0, 7 };
	const float test_2_1[] = { 0, 0.1, 0, 1.2, 0, 2.3, 0, 3.4, 0, 4.5, 0, 5.6, 0, 6.7, 0, 7.8 };

	// ---- 

	StructPack packer;
	int id0 = packer.addField(sizeof(int));
	int id1 = packer.addField(sizeof(float));

	packer.addPointer(id0, test_2_0, sizeof(int),   2 * sizeof(int));
	packer.addPointer(id1, test_2_1, sizeof(float), 2 * sizeof(float));

	void *result = packer(7);
	ASSERT_FALSE(result == NULL);

	struct test_0_t* result_data = (struct test_0_t*) result;

	for (size_t i = 0; i < 7; i++)
	{
		ASSERT_EQ(test_2_0[2 * i + 1], result_data[i].m_int);
		ASSERT_TRUE(result_data[i].m_float - test_2_1[2 * i + 1] < 0.00001);
	}
}

// ============================================================
/// Test with padded output for each field
#pragma pack(push, 1)
struct test_padding_t {
	union {
		int v;
		char p[16];
	} m_int;
	union {
		float v;
		char p[16];
	} m_float;
};


TEST(Struct, Padding)
{
	const int test_3_0[] = { 0, 1, 0, 2, 0, 3, 0, 4, 0, 5, 0, 6, 0, 7 };
	const float test_3_1[] = { 0, 0.1, 0, 1.2, 0, 2.3, 0, 3.4, 0, 4.5, 0, 5.6, 0, 6.7, 0, 7.8 };

	// --- 
	StructPack packer;

	packer.setFieldPadding(16);

	int id0 = packer.addField(sizeof(int));
	int id1 = packer.addField(sizeof(float));

	packer.addPointer(id0, test_3_0, sizeof(int), 2 * sizeof(int));
	packer.addPointer(id1, test_3_1, sizeof(float), 2 * sizeof(float));

	void *result = packer(7);
	ASSERT_FALSE(result == NULL);

	struct test_padding_t* result_data = (struct test_padding_t*) result;

	for (size_t i = 0; i < 7; i++)
	{
		ASSERT_EQ(test_3_0[2 * i + 1], result_data[i].m_int.v);
		ASSERT_TRUE(result_data[i].m_float.v - test_3_1[2 * i + 1] < 0.00001);

		// test zeroes
		for (size_t j = 4; j < 16; j++) {
			ASSERT_EQ(0, result_data[i].m_float.p[j]);
			ASSERT_EQ(0, result_data[i].m_int.p[j]);
		}
	}
}
#pragma pack(pop)

/// Test with padded/pack alignment output for the struct field
#pragma pack(push, 1)

TEST(Struct, PackAlignment)
{
	unsigned char in_data[256];
	for (size_t i = 0; i < 256; i++) {
		in_data[i] = i;
	}

	StructPack pack;
	pack.setRecordPadding(4);
	int id0 = pack.addField(1);
	pack.addPointer(id0, in_data, 0, 1);

	unsigned char* out_data = (unsigned char*) pack(256);

	for (size_t i = 0; i < 256; i++) {
		ASSERT_EQ(i, out_data[4 * i + 0]);
		ASSERT_EQ(0, out_data[4 * i + 1]);
		ASSERT_EQ(0, out_data[4 * i + 2]);
		ASSERT_EQ(0, out_data[4 * i + 3]);
	}
}

#pragma pack(pop)

// ============================================================
/// Test width padding overlaps a larger field
TEST(Struct, Padding_Overlap)
{
	/// @todo implement
	FAIL();
}

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

struct test_vertex_p16_t {
	union {
		struct {
			float x, y, z;
		} v;
		unsigned char p[16];
	};
};

TEST(Struct, PackVertices_FieldPadding) {
	StructPack packer;

	int id0 = packer.addField(3*4);
	packer.addPointer(id0, _cube_normals, 0, 3*4);
	packer.setFieldPadding(16);

	test_vertex_p16_t* data = (test_vertex_p16_t*)packer(4 * 6);

	ASSERT_EQ(1 * 16, packer.getRecordWidth());

	for (size_t i = 0; i < 4 * 6; i++) {
		ASSERT_TRUE(data[i].v.x - _cube_normals[3 * i + 0] < 0.000001);
		ASSERT_TRUE(data[i].v.y - _cube_normals[3 * i + 1] < 0.000001);
		ASSERT_TRUE(data[i].v.z - _cube_normals[3 * i + 2] < 0.000001);
	}
}

struct test_vertex_uv_p16_t {
	union {
		struct {
			float x, y, z;
		} vertex;
		unsigned char p[16];
	};

	union {
		struct {
			float x, y;
		} uv;
		unsigned char p[16];
	};
};

TEST(Struct, PackVertices_FieldPadding_Undefined_Ptr) {
	StructPack packer;

	int id0 = packer.addField(3 * 4);
	int id1 = packer.addField(2 * 4);

	packer.addPointer(id0, _cube_normals, 0, 3 * 4);

	packer.setFieldPadding(16);

	test_vertex_uv_p16_t* data = (test_vertex_uv_p16_t*)packer(4 * 6);

	ASSERT_EQ(2 * 16, packer.getRecordWidth());

	for (size_t i = 0; i < 4 * 6; i++) {
		ASSERT_TRUE(data[i].vertex.x - _cube_normals[3 * i + 0] < 0.000001);
		ASSERT_TRUE(data[i].vertex.y - _cube_normals[3 * i + 1] < 0.000001);
		ASSERT_TRUE(data[i].vertex.z - _cube_normals[3 * i + 2] < 0.000001);

		ASSERT_TRUE(data[i].uv.x < 0.000001);
		ASSERT_TRUE(data[i].uv.y < 0.000001);
	}
}

// ============================================================
/// Test for error
TEST(Struct, AddZeroWidth_Field)
{
	StructPack packer;
	ASSERT_THROW(packer.addField(0), ZeroOffsetException);
}

/// Test for error
TEST(Struct, AddNullptr)
{
	StructPack packer;
	int id0 = packer.addField(4);

	ASSERT_THROW(packer.addPointer(id0, NULL, 0, 0), NullPointerException);
}

/// Test for error
TEST(Struct, AddZeroStride)
{
	StructPack packer;
	int id0 = packer.addField(4);
	ASSERT_THROW(packer.addPointer(id0, &id0, 0, 0), ZeroOffsetException);
}


/// Test for error
TEST(Struct, AddInvalidID)
{
	StructPack packer;
	int id0 = packer.addField(4);
	ASSERT_THROW(packer.addPointer(id0+1, &id0, 0, 4), InvalidIDException);
}

/// Test for error
TEST(Struct, IdMismatch)
{
	StructPack packer;
	int id0 = packer.addField(4);
	int id1 = packer.addField(4);
	ASSERT_FALSE(id0 == id1);
}