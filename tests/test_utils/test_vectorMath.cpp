#include "stdafx.h"

#include <gtest/gtest.h>
#include "Assert.h"

#include "render/renderer.h"
#include "math/vector.h"

class VectorMathTest : public testing::Test {

public:
	void SetUp() {
	}

	void TearDown() {
	}

};


TEST_F(VectorMathTest, test_mul_vectors) {
	// given
	float3 v1 = float3(1, 2, 3);
	float3 v2 = float3(4, 5, 6);

	//wnen

	float3 res = v1 * v2;

	//then
	ASSERT_FLOAT3_EQ(float3(4, 10, 18), res);
}

TEST_F(VectorMathTest, test_mul_vectors2) {
	// given
	float3 v1 = float3(1, 2, 3);
	float3 v2 = float3(4, 5, 6);

	//wnen

	float3 res = v1;
	res *= v2;

	//then
	ASSERT_FLOAT3_EQ(float3(4, 10, 18), res);
}

TEST_F(VectorMathTest, test_mul_vectors_scalar) {
	// given
	float3 v1 = float3(1, 2, 3);
	float s = 3;

	//wnen

	float3 res = v1 * s;

	//then
	ASSERT_FLOAT3_EQ(float3(3, 6, 9), res);
}

TEST_F(VectorMathTest, test_mul_vectors_scalar2) {
	// given
	float3 v1 = float3(1, 2, 3);
	float s = 3;
	float z = 5;

	//wnen

	float3 res = v1;
	res *= s * z;

	//then
	ASSERT_FLOAT3_EQ(float3(15, 30, 45), res);
}