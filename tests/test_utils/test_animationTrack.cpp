#include "stdafx.h"

#include "animation/animation.h"

#include <gtest/gtest.h>

using namespace Grafkit;

const float data[10] = { 48.f, 46.f, 25.f, 2.f, 65.f, 14.f, 32.f, 65.f, 42.f, 1.f };
#define DATA_LEN (sizeof(data) / sizeof(*data))

class AnimationTrackTest : public testing::Test
{
public:
    void SetUp() override
    {
        channel = Animation::ChannelRef( new Animation::Channel());
        channel->Clear();
        for (int i = 0; i < DATA_LEN; i++) { channel->AddKey(Animation::Key(i, data[i])); }
    }

    void TearDown() override {
    }

    Animation::ChannelRef channel;
};


TEST_F(AnimationTrackTest, DeleteKeyTest)
{
    // Given
    size_t id = 0;

    // when
    channel->DeleteKey(id);

    // then
    Animation::Key key = channel->GetKey(id);
    ASSERT_NEAR(data[1], key.m_value, 0.000001); // first key was deleted
}

TEST_F(AnimationTrackTest, InsertBeforeFirstKeyTest)
{
    // Given
    const float t = -1.f;
    Animation::Key key = channel->GetKey(2);
    key.m_time = t;

    // when
    int id = channel->FindKeyIndex(t);
    channel->InsertKey(id + 1, key);

    // then
    Animation::Key v0;
    Animation::Key v1;
    float f = 0.f;
    int res = channel->FindKey(t, v0, v1, f);
    //ASSERT_NEAR(data[2], test_key.m_value, 0.000001); // second key was inserted

    ASSERT_EQ(0, res);

    ASSERT_NEAR(t, v0.m_time, 0.000001);
    ASSERT_NEAR(0., v1.m_time, 0.000001);

    ASSERT_NEAR(data[2], v0.m_value, 0.000001);
    ASSERT_NEAR(data[0], v1.m_value, 0.000001);
    ASSERT_NEAR(0, f, 0.000001);
}

TEST_F(AnimationTrackTest, InsertBetweenKeysTest)
{
    // Given
    size_t id = 1;
    Animation::Key key = channel->GetKey(2);

    // when
    channel->InsertKey(id, key);

    // then
    Animation::Key test_key = channel->GetKey(id);
    ASSERT_NEAR(data[2], test_key.m_value, 0.000001); // third key was inserted
}

TEST_F(AnimationTrackTest, InsertAfterLastTest)
{
    // Given
    size_t id = DATA_LEN;
    Animation::Key key = channel->GetKey(0);

    // when
    channel->InsertKey(id, key);

    // then
    Animation::Key test_key = channel->GetKey(id);
    ASSERT_NEAR(data[0], test_key.m_value, 0.000001); // the first key was inserted
}

TEST_F(AnimationTrackTest, FindBeforeFirstKeyTest)
{
    //given
    float t = -1;

    //when
    int32_t id = channel->FindKeyIndex(t);

    //then
    ASSERT_EQ(-1, id);
}

TEST_F(AnimationTrackTest, FindBetweenKeyTest)
{
    //given
    float t = 1.5;

    //when
    int32_t id = channel->FindKeyIndex(t);

    //then
    ASSERT_EQ(1, id);
}

TEST_F(AnimationTrackTest, FindAfterLastKeyTest)
{
    //given
    float t = DATA_LEN + 2;

    //when
    int32_t id = channel->FindKeyIndex(t);

    //then
    ASSERT_EQ(DATA_LEN - 1, id);
}

TEST_F(AnimationTrackTest, GetValueBeforeFirstKeyTest)
{
    // given
    float f = 0.f;
    const float t = -1.f;
    Animation::Key v0, v1;

    // when
    int res = channel->FindKey(t, v0, v1, f);

    // then
    ASSERT_EQ(0, res);

    ASSERT_NEAR(data[0], v0.m_value, 0.000001);
    ASSERT_NEAR(data[1], v1.m_value, 0.000001);
    ASSERT_NEAR(0, f, 0.000001);
}


TEST_F(AnimationTrackTest, GetValueOnFirstKeyTest)
{
    // given
    Animation::Key v0, v1;
    float f = 0.f;
    const float t = 0.f;

    // when
    int res = channel->FindKey(t, v0, v1, f);

    ASSERT_EQ(0, res);

    ASSERT_NEAR(data[0], v0.m_value, 0.000001);
    ASSERT_NEAR(data[1], v1.m_value, 0.000001);
    ASSERT_NEAR(0, f, 0.000001);
}

TEST_F(AnimationTrackTest, GetValueOnKeyTest)
{
    // given
    Animation::Key v0, v1;
    float f = 0.f;
    const float t = 1.f;

    // when
    int res = channel->FindKey(t, v0, v1, f);

    ASSERT_EQ(1, res);

    ASSERT_NEAR(data[1], v0.m_value, 0.000001);
    ASSERT_NEAR(data[2], v1.m_value, 0.000001);
    ASSERT_NEAR(0, f, 0.000001);
}

TEST_F(AnimationTrackTest, GetBetweenTwoKeysTest)
{
    // Given
    Animation::Key v0, v1;
    float f = 0.f;
    const float t = 1.5f;

    // When
    int res = channel->FindKey(t, v0, v1, f);

    // Then
    ASSERT_EQ(1, res);

    ASSERT_NEAR(data[1], v0.m_value, 0.000001);
    ASSERT_NEAR(data[2], v1.m_value, 0.000001);
    ASSERT_NEAR(.5, f, 0.000001);
}

TEST_F(AnimationTrackTest, GetValueOnLastKeyTest)
{
    // given
    Animation::Key v0, v1;
    float f = 0.f;
    const float t = static_cast<float>(DATA_LEN);

    // when
    int res = channel->FindKey(t, v0, v1, f);

    // then
    ASSERT_EQ(0, res);

    ASSERT_NEAR(data[DATA_LEN - 2], v0.m_value, 0.000001);
    ASSERT_NEAR(data[DATA_LEN - 1], v1.m_value, 0.000001);
    ASSERT_NEAR(1.f, f, 0.000001);
}


TEST_F(AnimationTrackTest, GetValueAfterLastKey) {
    // given
    Animation::Key v0, v1;
    float f = 0.f;
    const float t = static_cast<float>(DATA_LEN);

    // when
    int res = channel->FindKey(t, v0, v1, f);

    ASSERT_EQ(0, res);

    ASSERT_NEAR(data[DATA_LEN - 2], v0.m_value, 0.000001);
    ASSERT_NEAR(data[DATA_LEN - 1], v1.m_value, 0.000001);
    ASSERT_NEAR(1.f, f, 0.000001);
}

TEST_F(AnimationTrackTest, GetValueFromEmptyChannel)
{
    //given
    Animation::ChannelRef channel = Animation::ChannelRef(new Animation::Channel());
    channel->Clear();

    Animation::Key v0, v1;
    float f = 0.f;
    const float t = static_cast<float>(DATA_LEN);

    //when

    int res = channel->FindKey(t, v0, v1, f);

    //then
    ASSERT_EQ(-1, res);

}

TEST_F(AnimationTrackTest, GetValueBeforeOneKey)
{
    //given
    Animation::ChannelRef channel = Animation::ChannelRef(new Animation::Channel());

    Animation::Key v0, v1;
    float f = -1.f;
    const float t = static_cast<float>(DATA_LEN);

    //when

    int res = channel->FindKey(t, v0, v1, f);

    //then
    ASSERT_EQ(0, res);
    ASSERT_NEAR(0.f, f, 0.000001);
}

TEST_F(AnimationTrackTest, GetValueAfterOneKey)
{
    //given
    Animation::ChannelRef channel = Animation::ChannelRef(new Animation::Channel());

    Animation::Key v0, v1;
    float f = 1.f;
    const float t = static_cast<float>(DATA_LEN);

    //when

    int res = channel->FindKey(t, v0, v1, f);

    //then
    ASSERT_EQ(0, res);
    ASSERT_NEAR(0.f, f, 0.000001);

}