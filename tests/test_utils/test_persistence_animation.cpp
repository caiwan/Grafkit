/*
TCs for internal classes such as scene and so on
*/

#define _CRT_SECURE_NO_WARNINGS

#include "stdafx.h"

#include <gtest/gtest.h>

#include "animation/animation.h"

#include "utils/persistence/dynamics.h"
#include "utils/persistence/persistence.h"

#include "TestArchiver.h"
#include "TestClass_persistence.h"
#include "animation/actoranimation.h"

using namespace Grafkit;
using namespace FWdebugExceptions;
using namespace ArchivePersistent;

/*
    TEST_Fs if every used animation expected types could've been persisted
*/
class AnimationPersistenceTest : public testing::Test
{
public:
    ~AnimationPersistenceTest() {
    }

protected:
    void SetUp() override {
    }

    void TearDown() override {
    }


    void ValidateKey(const Animation::Key& expected, const Animation::Key& actual) const
    {
        ASSERT_EQ(expected.m_type, actual.m_type);
        ASSERT_FLOAT_EQ(expected.m_time, actual.m_time);
        ASSERT_FLOAT_EQ(expected.m_value, actual.m_value);
        ASSERT_FLOAT_EQ(expected.m_radius, actual.m_radius);
        ASSERT_FLOAT_EQ(expected.m_angle, actual.m_angle);
    }

    void CreateChannel(const Animation::ChannelRef &target, size_t keyCount) const
    {
        for (size_t i = 0; i < keyCount; ++i)
        {
            float f = i;
            Animation::Key key(
                float(f),
                float(rand()) / RAND_MAX,
                float(rand()) / RAND_MAX,
                float(rand()) / RAND_MAX,
                static_cast<Animation::KeyInterpolation_e>(rand() % Animation::KI_COUNT)
            );

            target->AddKey(key);
        }
    }

    void ValidatChannel(const Animation::ChannelRef &expected, const Animation::ChannelRef actual) const
    {
        ASSERT_TRUE(actual.Valid());
        ASSERT_EQ(expected->GetKeyCount(), actual->GetKeyCount());

        ASSERT_STREQ(expected->GetName().c_str(), actual->GetName().c_str());

        for (size_t i = 0; i < expected->GetKeyCount(); ++i)
        {
            Animation::Key expectedKey = expected->GetKey(i);
            Animation::Key axtualKey = actual->GetKey(i);
            ValidateKey(expectedKey, axtualKey);
        }
    }

    void CreateTrack(const Animation::TrackRef & track, size_t keyCount)
    {
        track->SetName("Track");
        track->CreateChannel("first");
        track->CreateChannel("second");
        track->CreateChannel("third");
        track->CreateChannel("fourth");

        CreateChannel(track->GetChannel(0), keyCount);
        CreateChannel(track->GetChannel(1), keyCount);
        CreateChannel(track->GetChannel(2), keyCount);
        CreateChannel(track->GetChannel(3), keyCount);
    }

    void ValidateTrack(const Animation::TrackRef &expected, const Animation::TrackRef actual) const
    {
        ASSERT_TRUE(actual.Valid());

        ASSERT_STREQ(expected->GetName().c_str(), actual->GetName().c_str());


        for (size_t i = 0; i < expected->GetChannelCount(); ++i)
        {
            Ref<Animation::Channel> expectedChannel = expected->GetChannel(i);
            Ref<Animation::Channel> actualChannel = actual->GetChannel(i);
            ValidatChannel(expectedChannel, actualChannel);
        }
    }

    void CreateAnimation(const AnimationRef &animation, size_t keyCount)
    {
        animation->SetName("Animation");
        CreateTrack(animation->GetTrack(0),keyCount);
        CreateTrack(animation->GetTrack(1), keyCount);
        CreateTrack(animation->GetTrack(2), keyCount);
    }

    void ValidateActrorAnimation(const AnimationRef &expected, const AnimationRef &actual ) const
    {
        ASSERT_TRUE(actual.Valid());

        ASSERT_STREQ(expected->GetName().c_str(), actual->GetName().c_str());
        ASSERT_STREQ(expected->GetUuid().c_str(), actual->GetUuid().c_str());

        ASSERT_EQ(expected->GetTrackCount(), actual->GetTrackCount());

        ValidateTrack(expected->GetTrack(0), actual->GetTrack(0));
        ValidateTrack(expected->GetTrack(1), actual->GetTrack(1));
        ValidateTrack(expected->GetTrack(2), actual->GetTrack(2));
    }
};


TEST_F(AnimationPersistenceTest, KeyTest)
{
    TestArchiver ar(256, true);

    // given
    Animation::Key key(.2, .3, .4, .5, Animation::KI_hermite);

    //when
    PERSIST_FIELD(ar, key);

    //then
    size_t pos = ar.GetCrsr();
    ar.ResetCrsr();
    ar.SetDirection(false);

    Animation::Key loadedKey;

    PERSIST_FIELD(ar, loadedKey);

    ValidateKey(key, loadedKey);
}

const size_t KEY_COUNT = 256;

TEST_F(AnimationPersistenceTest, ChannelTest)
{
    TestArchiver ar(16 * 4096, true);
    // given

    Animation::ChannelRef channel = new Animation::Channel();
    CreateChannel(channel, KEY_COUNT);

    // when
    channel->Store(ar);

    // then
    size_t pos = ar.GetCrsr();
    ar.ResetCrsr();
    ar.SetDirection(false);

    Animation::ChannelRef loadedChannel = Persistent::LoadT<Animation::Channel>(ar);

    ValidatChannel(channel, loadedChannel);
}

TEST_F(AnimationPersistenceTest, TrackTest)
{
    TestArchiver ar(64 * 4096, true);

    // given

    Animation::TrackRef track = new Animation::Track();
    CreateTrack(track, KEY_COUNT);

    // when
    track->Store(ar);

    // then
    size_t pos = ar.GetCrsr();
    ar.ResetCrsr();
    ar.SetDirection(false);

    Animation::TrackRef loadedTrack = Persistent::LoadT<Animation::Track>(ar);

    ValidateTrack(track, loadedTrack);
}

/*
    TEST_Fs if every used animation track types could've been persisted
*/

TEST_F(AnimationPersistenceTest, ActorAnimationTest)
{
    TestArchiver ar(16 * 4096, true);

    // given
    Ref<ActorAnimation> animation = new ActorAnimation();
    CreateAnimation(animation, KEY_COUNT);

    //when
    animation->Store(ar);

    //then
    size_t pos = ar.GetCrsr();
    ar.ResetCrsr();
    ar.SetDirection(false);

    Ref<ActorAnimation> loadedAnimation = Persistent::LoadT<ActorAnimation>(ar);
    ValidateActrorAnimation(animation, loadedAnimation);
}

// + multitrack
