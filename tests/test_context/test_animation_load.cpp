#include "stdafx.h"

#include <gtest/gtest.h>

#include "utils/AssetFile.h"

#include "experimental/scene.h"

#include "demo.h"
#include "context.h"
#include "schema.h"

#include "core/system.h"

#include "render/material.h"
#include "render/model.h"

#include "core/Music.h"
#include "animation/actoranimation.h"
#include "experimental/AnimationLoader.h"
#include "utils/persistence/archive.h"

// --- 

using namespace GkDemo;
using namespace Grafkit;

#define ASSET_ROOT "tests/assets/"
#define ANIMATION "testActorAnimation.anim"
#define ANIMATION_PATH ASSET_ROOT ANIMATION

namespace NSAnimationLoadTest
{
    class TestApplicationWindow : public System
    {
    public:
        explicit TestApplicationWindow(Renderer& render) : m_render(render)
        {
            InitializeWindows(320, 240);
            m_render.Initialize(m_window.getRealWidth(), m_window.getRealHeight(), true, this->m_window.getHWnd(), false);
        }

        ~TestApplicationWindow() { ShutdownWindows(); }

        int init() override { return 0; }

        int mainloop() override { return 0; }

        void release() override {
        }

    private:
        Renderer & m_render;
    };
}

using namespace NSAnimationLoadTest;

class AnimationLoadTest : public testing::Test
{
public:
    AnimationLoadTest() : m_window(nullptr)
        , m_context(nullptr)
    {
        m_window = new TestApplicationWindow(m_render);
        m_assetFactory = new FileAssetFactory(ASSET_ROOT);
    }

    ~AnimationLoadTest()
    {
        delete m_assetFactory;
        delete m_window;
    }

    void SetUp() override { m_context = new Context(m_render, m_assetFactory); }

    void TearDown() override
    {
        delete m_context;
        m_context = nullptr;
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
    Ref<Demo> m_demo;
    TestApplicationWindow* m_window;
    Context* m_context;

    Renderer m_render;
    IAssetFactory* m_assetFactory;

    ActorAnimationRef m_animation;
};

//

TEST_F(AnimationLoadTest, CreateEmptyTest)
{
    // given
    Ref<Resource<ActorAnimation>> resource = m_context->Load<Resource<ActorAnimation>>(new ActorAnimationLoader("testActorAnimation"));

    ASSERT_TRUE(resource);
    ASSERT_FALSE(*resource);

    // when
    m_context->DoPrecalc();

    // then
    ASSERT_TRUE(resource);
    ASSERT_TRUE(*resource);

    ActorAnimationRef animation = static_cast<Ref<ActorAnimation>>(*resource);

    ASSERT_EQ(3, animation->GetTrackCount());
    ASSERT_STREQ("Position", animation->GetTrack(0)->GetName().c_str());
    ASSERT_STREQ("Rotation", animation->GetTrack(1)->GetName().c_str());
    ASSERT_STREQ("Scaling", animation->GetTrack(2)->GetName().c_str());
}

TEST_F(AnimationLoadTest, LoadCacheTest)
{
    // given
    SaveTestData();
    Ref<Resource<ActorAnimation>> resource = m_context->Load<Resource<ActorAnimation>>(new ActorAnimationLoader("testActorAnimation", ANIMATION));

    ASSERT_TRUE(resource);
    ASSERT_FALSE(*resource);

    // when
    m_context->DoPrecalc();

    // then
    ASSERT_TRUE(resource);
    ASSERT_TRUE(*resource);

    ActorAnimationRef animation = static_cast<Ref<ActorAnimation>>(*resource);

    ValidateActrorAnimation(m_animation, animation);
}

TEST_F(AnimationLoadTest, DISABLED_SaveCacheTest)
{
    FAIL("Not implemented");
}