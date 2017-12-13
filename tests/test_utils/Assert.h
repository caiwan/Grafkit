#pragma once

#include <gtest/gtest.h>

/* Additional assertion tools */

#define _ASSERT_EPSILON 0.00001

#define ASSERT_FLOAT2_EQ(val1, val2)\
	ASSERT_NEAR(val1.x, val2.x, _ASSERT_EPSILON) << "X component got: " << val2.x  << " expected:" << val1.x;\
	ASSERT_NEAR(val1.y, val2.y, _ASSERT_EPSILON) << "Y component got: " << val2.y  << " expected:" << val1.y

#define ASSERT_FLOAT3_EQ(val1, val2)\
	ASSERT_FLOAT2_EQ(val1, val2);\
	ASSERT_NEAR(val1.z,  val2.z, _ASSERT_EPSILON) << "Z component got: " << val2.z  << " expected: " << val1.z;

#define ASSERT_FLOAT4_EQ(val1, val2)\
	ASSERT_FLOAT3_EQ(val1, val2);\
	ASSERT_NEAR(val1.w, val2.w, _ASSERT_EPSILON) << "W component got: " << val2.w  << " expected: " << val1.w;
