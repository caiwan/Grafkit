#include "stdafx.h"

#include "render/animation.h"

#include <gtest/gtest.h>

const float data[10] = { 48.f,	46.f,	25.f,	2.f,	65.f,	14.f,	32.f,	65.f,	42.f,	1.f };
#define DATA_LEN (sizeof(data) / sizeof(*data))
class AnimationTrackTest : public testing::Test {

public:
	void SetUp() {
		track.Clear();
		for (int i = 0; i < DATA_LEN; i++) {
			track.AddKey(Grafkit::Animation::FloatKey(1 + i, data[i]));
		}
	}

	void TearDown() {
	}

	Grafkit::Animation::FloatTrack track;
};


TEST_F(AnimationTrackTest, test_before_first_key) {
	float v0 = 0.f, v1 = 0.f, f = 0.f;
	int res = this->track.FindKey(0, v0, v1, f);

	ASSERT_EQ(0, res);

	ASSERT_NEAR(data[0], v0, 0.000001);
	ASSERT_NEAR(data[1], v1, 0.000001);
	ASSERT_NEAR(0, f, 0.000001);
}


TEST_F(AnimationTrackTest, test_on_first_key) {
	float v0 = 0.f, v1 = 0.f, f = 0.f;
	int res = this->track.FindKey(1., v0, v1, f);

	ASSERT_EQ(1, res);

	ASSERT_NEAR(data[0], v0, 0.000001);
	ASSERT_NEAR(data[1], v1, 0.000001);
	ASSERT_NEAR(0, f, 0.000001);
}

TEST_F(AnimationTrackTest, test_between_first_two_key) {
	float v0 = 0.f, v1 = 0.f, f = 0.f;
	int res = this->track.FindKey(1.5, v0, v1, f);

	ASSERT_EQ(1, res);

	ASSERT_NEAR(data[0], v0, 0.000001);
	ASSERT_NEAR(data[1], v1, 0.000001);
	ASSERT_NEAR(.5, f, 0.000001);
}

TEST_F(AnimationTrackTest, test_on_last_key) {
	float v0 = 0.f, v1 = 0.f, f = 0.f;
	int res = this->track.FindKey((float)(DATA_LEN), v0, v1, f);

	ASSERT_EQ(1, res);

	ASSERT_NEAR(data[DATA_LEN-2], v0, 0.000001);
	ASSERT_NEAR(data[DATA_LEN-1], v1, 0.000001);
	ASSERT_NEAR(1.f, f, 0.000001);
}

TEST_F(AnimationTrackTest, test_after_last_key) {
	float v0 = 0.f, v1 = 0.f, f = 0.f;
	int res = this->track.FindKey((float)(DATA_LEN+1), v0, v1, f);

	ASSERT_EQ(0, res);

	ASSERT_NEAR(data[DATA_LEN - 2], v0, 0.000001);
	ASSERT_NEAR(data[DATA_LEN - 1], v1, 0.000001);
	ASSERT_NEAR(1.f, f, 0.000001);
}
