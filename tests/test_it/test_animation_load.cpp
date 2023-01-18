#include "stdafx.h"

#include <gtest/gtest.h>

#include "utils/asset/AssetFile.h"

#include "application.h"

#include "scene/scene.h"

#include "render/material.h"
#include "render/model.h"

#include "core/Music.h"
#include "animation/actoranimation.h"
#include "resource/loaders/AnimationLoader.h"
#include "utils/persistence/archive.h"

// --- 

using namespace Grafkit;

#define ASSET_ROOT "tests/assets/"
#define ANIMATION_ROOT "animation/"
#define ANIMATION "testActorAnimation.anim"
#define ANIMATION_PATH ASSET_ROOT ANIMATION

class AnimationLoadTest : public testing::Test //, public ResourceManager
{
public:
    AnimationLoadTest() : m_app(nullptr)
    {
        m_assetFactory = new FileAssetFactory(ASSET_ROOT);
        m_app = new Testing::TestApplicationContext(m_render, m_assetFactory);
    }

    ~AnimationLoadTest()
    {
        delete m_assetFactory;
        delete m_app;
    }

    void SetUp() override { 
    }

    void TearDown() override
    {
    }


    void SaveTestData()
    {
        m_animation = new ActorAnimation();
        m_animation->Initialize();

        for (size_t i = 0; i < m_animation->GetTrackCount(); i++)
        {
            Animation::TrackRef track = m_animation->GetTrack(i);
            for (size_t j = 0; j < track->GetChannelCount(); j++)
            {
                Animation::ChannelRef channel = track->GetChannel(j);
                for (size_t k = 0; k < 256; k++)
                {
                    Animation::Key key(float(k), rand() / RAND_MAX, rand() / RAND_MAX, rand() / RAND_MAX, Animation::KeyInterpolation_e(k % Animation::KI_COUNT));
                    channel->AddKey(key);
                }
            }

        }

        FILE* fp;
        errno_t err = fopen_s(&fp, ANIMATION_PATH, "wb");
        assert(0 == err);

        ArchiveFile archive(fp, true);
        m_animation->Store(archive);
        fclose(fp);
    }

    void ValidateKey(const Animation::Key& expected, const Animation::Key& actual) const
    {
        ASSERT_EQ(expected.m_type, actual.m_type);
        ASSERT_FLOAT_EQ(expected.m_time, actual.m_time);
        ASSERT_FLOAT_EQ(expected.m_value, actual.m_value);
        ASSERT_FLOAT_EQ(expected.m_radius, actual.m_radius);
        ASSERT_FLOAT_EQ(expected.m_angle, actual.m_angle);
    }

    void ValidateChannel(const Animation::ChannelRef &expected, const Animation::ChannelRef actual) const
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

    void ValidateTrack(const Animation::TrackRef &expected, const Animation::TrackRef actual) const
    {
        ASSERT_TRUE(actual.Valid());

        ASSERT_STREQ(expected->GetName().c_str(), actual->GetName().c_str());


        for (size_t i = 0; i < expected->GetChannelCount(); ++i)
        {
            Ref<Animation::Channel> expectedChannel = expected->GetChannel(i);
            Ref<Animation::Channel> actualChannel = actual->GetChannel(i);
            ValidateChannel(expectedChannel, actualChannel);
        }
    }

    void ValidateActrorAnimation(const AnimationRef &expected, const AnimationRef &actual) const
    {
        ASSERT_TRUE(actual.Valid());

        ASSERT_STREQ(expected->GetName().c_str(), actual->GetName().c_str());
        ASSERT_STREQ(expected->GetUuid().c_str(), actual->GetUuid().c_str());

        ASSERT_EQ(expected->GetTrackCount(), actual->GetTrackCount());

        ValidateTrack(expected->GetTrack(0), actual->GetTrack(0));
        ValidateTrack(expected->GetTrack(1), actual->GetTrack(1));
        ValidateTrack(expected->GetTrack(2), actual->GetTrack(2));
    }

protected:
    Testing::TestApplicationContext* m_app;

    Renderer m_render;
    IAssetFactory* m_assetFactory;

    ActorAnimationRef m_animation;
};

//

TEST_F(AnimationLoadTest, CreateEmptyTest)
{
    // given
    Ref<Resource<ActorAnimation>> resource = m_app->Load<Resource<ActorAnimation>>(new ActorAnimationLoader("testActorAnimation"));

    ASSERT_TRUE(resource);
    ASSERT_FALSE(*resource);

    // when
    m_app->DoPrecalc();

    // then
    ASSERT_TRUE(resource);
    ASSERT_TRUE(*resource);

    ActorAnimationRef animation = static_cast<Ref<ActorAnimation>>(*resource);

    ASSERT_EQ(3, animation->GetTrackCount());
    ASSERT_STREQ("Position", animation->GetTrack(0)->GetName().c_str());
    ASSERT_STREQ("Rotation", animation->GetTrack(1)->GetName().c_str());
    ASSERT_STREQ("Scaling", animation->GetTrack(2)->GetName().c_str());
}

TEST_F(AnimationLoadTest, LoadAnimation)
{
    // given
    SaveTestData();
    Ref<Resource<ActorAnimation>> resource = m_app->Load<Resource<ActorAnimation>>(new ActorAnimationLoader("testActorAnimation", ANIMATION));

    ASSERT_TRUE(resource);
    ASSERT_FALSE(*resource);

    // when
    m_app->DoPrecalc();

    // then
    ASSERT_TRUE(resource);
    ASSERT_TRUE(*resource);

    ActorAnimationRef animation = static_cast<Ref<ActorAnimation>>(*resource);

    ValidateActrorAnimation(m_animation, animation);
}

// Load dome pre-existing naimation, due a previous error 

namespace
{
    const char *animationFileNames[] = {
        "19ff02df-04d5-49a4-a091-bd92f185316b.1",
        "353732c5-de20-4180-831c-481b0d3caa460.1",
        "ad29899f-5424-49dd-b821-3a9644dfd41a.1"
    };

    const char *animationUuids[] = {
        "19ff02df-04d5-49a4-a091-bd92f185316b",
        "353732c5-de20-4180-831c-481b0d3caa460",
        "ad29899f-5424-49dd-b821-3a9644dfd41a"
    };

    const char *trackNames[] = {
        "Position",
        "Rotation",
        "Scaling"
    };
}

TEST_F(AnimationLoadTest, LoadPreexistingAnimation)
{
    // given
    
    std::list<Ref<Resource<ActorAnimation>>> resources;
    for (std::string fileName : animationFileNames)
    {
        std::string path = ANIMATION_ROOT + fileName;
        resources.push_back(m_app->Load<Resource<ActorAnimation>>(new ActorAnimationLoader("", path.c_str(), "" )));
    }

    // when
    m_app->DoPrecalc();

    // then
    size_t index = 0;
    for (Ref<Resource<ActorAnimation>>resource : resources )
    {
        ASSERT_TRUE(resource) << "Animation id:" << index;
        ASSERT_TRUE(*resource) << "Animation id:" << index;

        ActorAnimationRef animation = static_cast<Ref<ActorAnimation>>(*resource);
    
        size_t trackCount = animation->GetTrackCount();
        ASSERT_EQ(3, trackCount) << "Animation id:" << index;
        for (size_t i = 0; i< trackCount; i++)
        {
            Ref<Animation::Track> track = animation->GetTrack(i);
            ASSERT_TRUE(track);
            ASSERT_STREQ(trackNames[i], track->GetName().c_str()) << "Animation id:" << index << "Track id:" << i;

            size_t channelCount = track->GetChannelCount();
            ASSERT_NE(0, channelCount); 
            for (size_t j= 0; j< channelCount;j++)
            {
                Ref<Animation::Channel> channel = track->GetChannel(j);
                ASSERT_TRUE(channel);
                ASSERT_NE(0, channel->GetKeyCount());
            }
        }

        index++;
    }

}
