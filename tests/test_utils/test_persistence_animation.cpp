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
#include "testClass_persistence.h"

using namespace Grafkit;
using namespace FWdebugExceptions;
using namespace ArchivePersistent;
/*
    Tests if every used animation key types could've been persisted
*/

#define ASSERT_KEY_EQ(val1, val2)\
	ASSERT_EQ(val1.m_type, val2.m_type); \
	ASSERT_FLOAT_EQ(val1.m_time, val2.m_time); \
    ASSERT_FLOAT_EQ(val1.m_value, val2.m_value); \
    ASSERT_FLOAT_EQ(val1.m_radius, val2.m_radius); \
    ASSERT_FLOAT_EQ(val1.m_angle, val2.m_angle);

TEST(Persistent_Animation, AnimationKeyPersistTest)
{
    TestArchiver ar(256, true);

    // given
    Animation::Key key(.2, .3, .4, .5, Animation::KI_hermite);
    
    //when
    PERSIST_FIELD(ar, key);

    //then
    ar.ResetCrsr();
    ar.SetDirection(false);

    Animation::Key test_key;

    PERSIST_FIELD(ar, test_key);

    ASSERT_KEY_EQ(key, test_key);
    ASSERT_FLOAT_EQ(key.m_value, test_key.m_value);
}

#define KEY_COUNT 256

TEST(Persistent_Animation, AnimationChannelPersistTest)
{
    TestArchiver ar(16*256, true);
    // given

    // when

    // then

}

TEST(Persistent_Animation, AnimationTrackPersistTest)
{
    TestArchiver ar(16 * 256, true);

    // given
    
    // when
    
    // then

}

/*
    Tests if every used animation track types could've been persisted
*/

TEST(Persistent_Animation, ActorAnimationPersistTest)
{
 
#if 0
    TestArchiver ar(16 * 4096, true);

    // given
    Animation::Channel track;

    for (int i = 0; i < 256; ++i)
    {
        float f = 1. / i;
        Animation::Key key(f, f/2, f/3, f/4, static_cast<Animation::KeyInterpolation_e>(i % Animation::KI_COUNT));

        track.AddKey(key);
    }

    //when
    track.serialize(ar);

    //then
    ar.ResetCrsr();
    ar.SetDirection(false);

    AnimationRef testAnimation = new Track();
    testAnimation.serialize(ar);

    ASSERT_EQ(track.GetKeyCount(), testAnimation.GetKeyCount());

    for (int i = 0; i < 256; ++i)
    {
        Animation::Key key = track.GetKey(i);
        Animation::Key test_key = testAnimation.GetKey(i);

        ASSERT_KEY_EQ(key, test_key);
        ASSERT_FLOAT_EQ(key.m_value, test_key.m_value);
    }

#endif
}

// + multitrack
