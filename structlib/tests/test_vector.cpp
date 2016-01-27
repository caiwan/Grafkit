#include "gtest/gtest.h"
#include "std_vector.h"

TEST(Vector, Append) 
{
	FWutils::vector<unsigned int> v;

	for (size_t i = 0; i < 100; i++) {
		v << i;
	}

	ASSERT_EQ(100, v.size());

	for (size_t i = 0; i < 100; i++) {
		ASSERT_EQ(i, v[i]);
	}
}

TEST(Vector, Initisize)
{
	FWutils::vector<unsigned int> v(100);
	ASSERT_EQ(100, v.size());

	for (size_t i = 0; i < 100; i++) {
		v[i] = i;
	}

	for (size_t i = 0; i < 100; i++) {
		ASSERT_EQ(i, v[i]);
	}

}