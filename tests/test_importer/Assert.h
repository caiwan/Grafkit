#pragma once

#include <gtest/gtest.h>

/* Additional assertion tools */

#if 0

#define ASSERT_FLOAT2_EQ(val1, val2)\
	ASSERT_FLOAT_EQ(val1.x, val2.x);\
	ASSERT_FLOAT_EQ(val1.y, val2.y)

#define ASSERT_FLOAT3_EQ(val1, val2)\
	ASSERT_FLOAT2_EQ(val1, val2);\
	ASSERT_FLOAT_EQ(val1.z, val2.z)

#define ASSERT_FLOAT4_EQ(val1, val2)\
	ASSERT_FLOAT3_EQ(val1, val2);\
	ASSERT_FLOAT_EQ(val1.w, val2.w)

#else

#define ASSERT_FLOAT2_EQ(val1, val2)\
	ASSERT_NEAR(val1.x, val2.x, 0.00001) << "X component got: " << val2.x  << " expected:" << val1.x;\
	ASSERT_NEAR(val1.y, val2.y, 0.00001) << "Y component got: " << val2.y  << " expected:" << val1.y

#define ASSERT_FLOAT3_EQ(val1, val2)\
	ASSERT_FLOAT2_EQ(val1, val2);\
	ASSERT_NEAR(val1.z,  val2.z, 0.00001, ) << "Z component got: " << val2.z  << " expected: " << val1.z;

#define ASSERT_FLOAT4_EQ(val1, val2)\
	ASSERT_FLOAT3_EQ(val1, val2);\
	ASSERT_NEAR(val1.w, val2.w, 0.00001) << "W component got: " << val2.w  << " expected: " << val1.w;

#endif