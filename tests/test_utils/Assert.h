#pragma once

#include <gtest/gtest.h>

/* Additional assertion tools */

#define ASSERT_FLOAT2_EQ(val1, val2)\
	ASSERT_FLOAT_EQ(val1.x, val2.x);\
	ASSERT_FLOAT_EQ(val1.y, val2.y)

#define ASSERT_FLOAT3_EQ(val1, val2)\
	ASSERT_FLOAT2_EQ(val1, val2);\
	ASSERT_FLOAT_EQ(val1.z, val2.z)

#define ASSERT_FLOAT4_EQ(val1, val2)\
	ASSERT_FLOAT3_EQ(val1, val2);\
	ASSERT_FLOAT_EQ(val1.w, val2.w)
