/*
TCs for internal classes such as scene and so on
*/

#define _CRT_SECURE_NO_WARNINGS

#include "stdafx.h"

#include <gtest/gtest.h>

#include "render/SceneGraph.h"
#include "render/Mesh.h"
#include "render/Model.h"
#include "render/Material.h"
#include "render/Camera.h"
#include "render/Light.h"

#include "animation/actoranimation.h"

#include "core/logger.h"

#include "utils/persistence/archive.h"

using namespace Grafkit;
using namespace FWdebugExceptions;

/* ****************************************************************************************************************************************
* Test for some base types
**************************************************************************************************************************************** */

template <class T>
void TestArchive(const T& testValue)
{
    // given
    std::stringstream s;
    Archive a(std::make_unique<Stream<std::stringstream>>(s));
    T value(testValue);

    //when
    a << testValue;
    ASSERT_EQ(value, testValue);

    // then
    value = T();
    ASSERT_NE(value, testValue);

    a >> value;
    ASSERT_EQ(value, testValue);
}

TEST(PersistenceInternal, FloatVectors)
{
    TestArchive<float2>({1,2});
    TestArchive<float3>({1,2,3});
    TestArchive<float4>({1,2,3,4});
}

TEST(PersistenceInternal, Matrix) { TestArchive<Matrix>({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16}); }

TEST(PersistenceInternal, Quaternion) { TestArchive<Quaternion>({1, 2, 3, 4}); }


template <class C>
void TestObject(const Ref<C>& object)
{
    Log::Logger().Info("Given T<%s>", typeid(C).name());

    // given
    //Ref<const C> object = testValue;//new C();

    std::stringstream s;
    Archive a(std::make_unique<Stream<std::stringstream>>(s));

    //when
    a << object;

    // then
    Ref<C> readObject;

    a >> readObject;

    ASSERT_TRUE(readObject.Valid());
    ASSERT_TRUE(object.Get() != readObject.Get());

    // Test Object traits 
    Object* objBase = dynamic_cast<Object*>(object.Get());
    Object* readObjBase = dynamic_cast<Object*>(readObject.Get());

    if (objBase && readObjBase)
    {
        ASSERT_STREQ(objBase->GetName().c_str(), readObjBase->GetName().c_str());
        ASSERT_EQ(objBase->GetUuid(), readObjBase->GetUuid());
    }
    else { Log::Logger().Warn("T<%s> has no Object traits", typeid(C).name()); }
}

TEST(PersistenceInternal, Actor) { TestObject<Actor>(new Actor()); }
TEST(PersistenceInternal, Model) { TestObject<Model>(new Model()); }
TEST(PersistenceInternal, Material) { TestObject<Material>(new Material()); }
TEST(PersistenceInternal, Mesh) { TestObject<Mesh>(new Mesh()); }
TEST(PersistenceInternal, Camera) { TestObject<Camera>(new Camera()); }
TEST(PersistenceInternal, Light) { TestObject<Light>(new Light()); }

TEST(PersistenceInternal, ActorAnimation)
{
    Ref<ActorAnimation> animation = new ActorAnimation();
    animation->Initialize();
    TestObject<ActorAnimation>(animation);
}
