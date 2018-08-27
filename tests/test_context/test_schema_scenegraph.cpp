#include "stdafx.h"

#include <gtest/gtest.h>

#include "utils/AssetFile.h"
#include "utils/ResourceManager.h"

#include "demo.h"
#include "context.h"
#include "schema.h"

#include "render/actor.h"

#include "render/material.h"
#include "render/mesh.h"
#include "render/model.h"
#include "render/camera.h"
#include "render/light.h"

#include "core/Music.h"
#include "core/system.h"

#include "animation/actoranimation.h"

// --- 

using namespace GkDemo;
using namespace Grafkit;

#define JSON_PATH "schema.json"

// --- 

namespace NSSchemaTest
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

using namespace NSSchemaTest;


class SchemaTest : public testing::Test
{
public:
    SchemaTest() : m_context(nullptr)
        , m_window(nullptr)
        , m_assetFactory(nullptr)
    {
        m_window = new TestApplicationWindow(m_render);
        m_assetFactory = new FileAssetFactory("tests/assets/");
    }

    ~SchemaTest()
    {
        delete m_assetFactory;
        delete m_window;
    }

    void SetUp() override { m_context = new Context(m_render, m_assetFactory); }

    void TearDown() override {
        delete m_context;
    }

    void BuildDemo()
    {
        try
        {
            SchemaBuilder builder;

            // shit
            IResourceManager* resman = dynamic_cast<IResourceManager*>(m_context);

            builder.LoadFromAsset(m_assetFactory->Get(JSON_PATH), resman);
            m_context->DoPrecalc();
            m_demo = builder.GetDemo();
            builder.Initialize(resman);
            m_demo->Initialize(m_render);
        }
        catch (std::exception& e)
        {
            FAIL() << e.what();
        }
    }

    template <class T>
    Ref<T> SafeGetObject(const char* uuid)
    {
        assert(uuid);
        IResourceManager* resman = dynamic_cast<IResourceManager*>(m_context);
        assert(resman);
        Ref<Resource<T>> tResource = resman->GetByUuid<Resource<T>>(uuid);
        if (!tResource)
            return nullptr;
        if (!*tResource)
            return nullptr;
        return tResource->Get();
    }

    template <class T>
    Ref<Resource<T>> SafeGetResource(const char* uuid)
    {
        assert(uuid);
        Ref<Resource<T>> tResource = m_context->GetByUuid<Resource<T>>(uuid);
        if (!tResource)
            return nullptr;
        return tResource;
    }

protected:
    Ref<Demo> m_demo;
    Context* m_context;
    TestApplicationWindow *m_window;
    Renderer m_render;
    IAssetFactory* m_assetFactory;
};

// ============================================================================================

TEST_F(SchemaTest, JsonLoad)
{
    // given: context
    // when
    this->BuildDemo();
    // then
    ASSERT_TRUE(m_demo.Valid());
    ASSERT_EQ(1, m_demo->GetSceneCount());
    ASSERT_TRUE(m_demo->GetScene(0));
    ASSERT_TRUE(m_demo->GetMusic());
    ASSERT_EQ(1, m_demo->GetAnimationCount());
    ASSERT_TRUE(m_demo->GetAnimation(0));
}

// ============================================================================================

namespace Uuids
{
    const char* modelUuid = "a0f30c19-ecdb-4f22-8cf5-f908c6250af9";
    const char* meshUuid = "00751e45-a404-4955-80f4-b17a2c30a027";
    const char* materialUuid = "733e8988-32c3-411f-b877-dca11c761b08";

    const char* camera1Uuid = "7e90b629-8499-481f-8887-8de6c5392a1a";
    const char* camera2Uuid = "54e1f5b3-61e2-4413-8d80-edc46148a976";

    const char* lightUuid = "2bfbc0bc-f71e-4ed7-9b0a-36c50fab805c";

    const char* vsUuid = "43ae2c7a-b8e7-4eb0-a888-7ce1c2fa90a1";
    const char* psUuid = "5f72e9aa-93f5-4d86-ada8-9543b625337e";
}

TEST_F(SchemaTest, AssetMaterialLoadTest)
{
    // given: 
    this->BuildDemo();

    // when
    MaterialRef material = SafeGetObject<Material>(Uuids::materialUuid);

    // then
    ASSERT_TRUE(material);
    ASSERT_STREQ("PBRCubeMaterial", material->GetName().c_str());
}

TEST_F(SchemaTest, AssetMeshLoadTest)
{
    // given: context and ... 
    this->BuildDemo();

    // when
    MeshRef mesh = SafeGetObject<Mesh>(Uuids::meshUuid);

    // then
    ASSERT_TRUE(mesh);
    ASSERT_STREQ("CubeMesh", mesh->GetName().c_str());

    ASSERT_NE(nullptr, mesh->GetPointer("POSITION"));
    ASSERT_NE(nullptr, mesh->GetPointer("TEXCOORD"));
    ASSERT_NE(nullptr, mesh->GetPointer("NORMAL"));
    ASSERT_EQ(6 * 6, mesh->GetIndexCount());
    ASSERT_NE(nullptr, mesh->GetIndices());
}

TEST_F(SchemaTest, AssetModelLoadTest)
{
    // given: context and ... 
    this->BuildDemo();

    // when
    ModelRef model = SafeGetObject<Model>(Uuids::modelUuid);
    MeshRef mesh = SafeGetObject<Mesh>(Uuids::meshUuid);
    MaterialRef material = SafeGetObject<Material>(Uuids::materialUuid);

    // then
    ASSERT_TRUE(model);
    ASSERT_STREQ("Cube", model->GetName().c_str());
    ASSERT_EQ(mesh, model->GetMesh());
    ASSERT_EQ(material, model->GetMaterial());
}

TEST_F(SchemaTest, AssetCameraLoadTest)
{
    // given: context and ... 
    this->BuildDemo();

    // when
    CameraRef camera1 = SafeGetObject<Camera>(Uuids::camera1Uuid);
    CameraRef camera2 = SafeGetObject<Camera>(Uuids::camera2Uuid);

    // then
    ASSERT_TRUE(camera1);
    ASSERT_TRUE(camera2);
    ASSERT_NE(camera1, camera2);

    ASSERT_STREQ("Camera01", camera1->GetName().c_str());
    ASSERT_STREQ("Camera02", camera2->GetName().c_str());
}

TEST_F(SchemaTest, AssetLightLoadTest)
{
    // given: context and ... 
    this->BuildDemo();

    // when
    LightRef light = SafeGetObject<Light>(Uuids::lightUuid);

    // then
    ASSERT_TRUE(light);
    ASSERT_STREQ("Light01", light->GetName().c_str());
}

TEST_F(SchemaTest, AssetVShaderLoadTest)
{
    // given: context and ... 
    this->BuildDemo();

    // when
    ShaderResRef shader = SafeGetResource<Shader>(Uuids::vsUuid);

    // then
    ASSERT_TRUE(shader);
    ASSERT_STREQ("vertexShader", shader->GetName().c_str());
}

TEST_F(SchemaTest, AssetPShaderLoadTest)
{
    // given: context and ... 
    this->BuildDemo();

    // when
    ShaderResRef shader = SafeGetResource<Shader>(Uuids::psUuid);

    // then
    ASSERT_TRUE(shader);
    ASSERT_STREQ("pixelShader", shader->GetName().c_str());
}

// ============================================================================================

namespace Uuids
{
    const char* rootActorUuid = "1c3ee37b-74ed-4147-9cf1-0df6de5fc0e5";
    const char* cameraActorUuid = "1e57c5da-0d9c-4b3a-b257-643d5b2c994a";
    const char* camera1ActorUuid = "0b61b7d6-c144-42e9-aceb-e9dbf683f9f7";
    const char* camera2ActorUuid = "f8b9000d-c87e-4805-ae2e-70529c94797d";
    const char* lightActorUuid = "f95ceb23-3144-4522-8275-8c1433f50595";
    const char* cubeActorUuid = "3bd7e8db-e851-4db2-b333-16e0b3f66019";
}

TEST_F(SchemaTest, ActorMeshTest)
{
    // given
    this->BuildDemo();

    //when
    ActorRef cubeActor = SafeGetObject<Actor>(Uuids::cubeActorUuid);
    ModelRef model = SafeGetObject<Model>(Uuids::modelUuid);

    // then 
    ASSERT_TRUE(cubeActor);
    ASSERT_TRUE(model);

    ASSERT_STREQ("CubeActor", cubeActor->GetName().c_str());

    ASSERT_EQ(1, cubeActor->GetEntityCount());

    ASSERT_EQ(model.Get(), cubeActor->GetEntity(0).Get());
}

TEST_F(SchemaTest, ActorCameraTest)
{
    // given
    this->BuildDemo();

    //when
    ActorRef cameraActor = SafeGetObject<Actor>(Uuids::cameraActorUuid);

    ActorRef camera1Actor = SafeGetObject<Actor>(Uuids::camera1ActorUuid);
    ActorRef camera2Actor = SafeGetObject<Actor>(Uuids::camera2ActorUuid);
    CameraRef camera1 = SafeGetObject<Camera>(Uuids::camera1Uuid);
    CameraRef camera2 = SafeGetObject<Camera>(Uuids::camera2Uuid);

    // then 
    ASSERT_TRUE(cameraActor);
    ASSERT_STREQ("CameraActor", cameraActor->GetName().c_str());

    ASSERT_EQ(2, cameraActor->GetChildrenCount());

    ASSERT_TRUE(camera1Actor);
    ASSERT_TRUE(camera2Actor);

    ASSERT_EQ(camera1Actor, cameraActor->GetChild(0));
    ASSERT_EQ(camera2Actor, cameraActor->GetChild(1));

    ASSERT_TRUE(camera1);
    ASSERT_TRUE(camera2);

    ASSERT_NE(camera1Actor, camera2Actor);

    ASSERT_STREQ("Camera01", camera1Actor->GetName().c_str());
    ASSERT_STREQ("Camera02", camera2Actor->GetName().c_str());

    ASSERT_EQ(1, camera1Actor->GetEntityCount());
    ASSERT_EQ(1, camera2Actor->GetEntityCount());

    ASSERT_EQ(camera1.Get(), camera1Actor->GetEntity(0).Get());
    ASSERT_EQ(camera2.Get(), camera2Actor->GetEntity(0).Get());
}

TEST_F(SchemaTest, ActorLightTest)
{
    // given
    this->BuildDemo();

    //when
    ActorRef lightActor = SafeGetObject<Actor>(Uuids::lightActorUuid);
    LightRef light = SafeGetObject<Light>(Uuids::lightUuid);

    // then 
    ASSERT_TRUE(lightActor);
    ASSERT_STREQ("LightActor", lightActor->GetName().c_str());

    ASSERT_TRUE(light);

    ASSERT_EQ(light.Get(), lightActor->GetEntity(0).Get());
}

/*
 * RootActor -> CameraActor
 * RootActor -> LightActor
 * RootActor -> CubeActor
 *
 * CameraActor -> Camera01
 * CameraActor -> Camera02
 */

TEST_F(SchemaTest, ActorSchemaTest)
{
    // given
    BuildDemo();

    // when
    ActorRef rootActor = SafeGetObject<Actor>(Uuids::rootActorUuid);
    ActorRef cameraActor = SafeGetObject<Actor>(Uuids::cameraActorUuid);
    ActorRef lightActor = SafeGetObject<Actor>(Uuids::lightActorUuid);
    ActorRef cubeActor = SafeGetObject<Actor>(Uuids::cubeActorUuid);

    // then
    ASSERT_TRUE(rootActor);
    ASSERT_STREQ("RootActor", rootActor->GetName().c_str());

    // the names for the rest are already checked
    ASSERT_TRUE(cameraActor);
    ASSERT_TRUE(lightActor);
    ASSERT_TRUE(cubeActor);

    ASSERT_EQ(3, rootActor->GetChildrenCount());
    ASSERT_EQ(cameraActor, rootActor->GetChild(0));
    ASSERT_EQ(lightActor, rootActor->GetChild(1));
    ASSERT_EQ(cubeActor, rootActor->GetChild(2));
}

// ============================================================================================

namespace Uuids
{
    const char* sceneGraphUuid = "858543a6-bf2d-42b5-93b9-0c73cec79369";
    const char* sceneUuid = "dd0900ab-d654-4d65-9960-42c0633c535e";
}

TEST_F(SchemaTest, DemoSceneTest)
{
    // given
    BuildDemo();

    // when
    SceneResRef sceneRes = SafeGetResource<Scene>(Uuids::sceneUuid);
    SceneGraphRef sceneGraph = SafeGetObject<SceneGraph>(Uuids::sceneGraphUuid);
    ActorRef rootActor = SafeGetObject<Actor>(Uuids::rootActorUuid);

    // then
    ASSERT_TRUE(sceneRes);
    ASSERT_TRUE(*sceneRes);

    ASSERT_TRUE(sceneGraph);
    ASSERT_TRUE(rootActor);

    ASSERT_EQ(sceneRes, m_demo->GetScene(0));

    ASSERT_STREQ("TestScene", (*sceneRes)->GetName().c_str());
    ASSERT_STREQ("TestSceneGraph", sceneGraph->GetName().c_str());

    ASSERT_EQ(sceneGraph, (*sceneRes)->GetSceneGraph());
    ASSERT_EQ(rootActor, sceneGraph->GetRootNode());
}


TEST_F(SchemaTest, DemoPShaderTest)
{
    // given
    BuildDemo();

    // when
    ShaderResRef shader = SafeGetResource<Shader>(Uuids::psUuid);

    // then
    ASSERT_TRUE(shader);

    ASSERT_EQ(shader, m_demo->GetPs());
}

TEST_F(SchemaTest, DemoVShaderTest)
{
    // given
    BuildDemo();

    // when
    SceneResRef sceneRes = SafeGetResource<Scene>(Uuids::sceneUuid);
    ShaderResRef shader = SafeGetResource<Shader>(Uuids::vsUuid);

    // then
    ASSERT_TRUE(sceneRes);
    ASSERT_TRUE(*sceneRes);
    ASSERT_TRUE(shader);

    ASSERT_EQ(shader, m_demo->GetVs());
}

// ============================================================================================

namespace Uuids
{
    const char* rootActorAnimationUuid = "f7793948-3fdc-4182-bca6-c58917de46d7";
    const char* cameraActorAnimationUuid = "e069b70d-ddf7-471e-84ac-ac30e1cd4f94";
    const char* camera1ActorAnimationUuid = "f1932797-1119-43a0-89c3-d0c08f4b8288";
    const char* camera2ActorAnimationUuid = "ad29899f-5424-49dd-b821-3a9644dfd41a";
    const char* lightActorAnimationUuid = "19ff02df-04d5-49a4-a091-bd92f185316b";
    const char* cubeActorAnimationUuid = "353732c5-de20-4180-831c-481b0d3caa460";

    const char * demoAnimationUuid = "4f545fcf-9713-4055-841b-e0b2415a125b";

}


TEST_F(SchemaTest, ActorAnimations)
{
    // given
    BuildDemo();

    // when
    ActorRef rootActor = SafeGetObject<Actor>(Uuids::rootActorUuid);
    ActorRef cameraActor = SafeGetObject<Actor>(Uuids::cameraActorUuid);
    ActorRef camera1Actor = SafeGetObject<Actor>(Uuids::camera1ActorUuid);
    ActorRef camera2Actor = SafeGetObject<Actor>(Uuids::camera2ActorUuid);
    ActorRef lightActor = SafeGetObject<Actor>(Uuids::lightActorUuid);
    ActorRef cubeActor = SafeGetObject<Actor>(Uuids::cubeActorUuid);

    ActorAnimationResRef rootActorAnimation = SafeGetResource<ActorAnimation>(Uuids::rootActorAnimationUuid);
    ActorAnimationResRef cameraActorAnimation = SafeGetResource<ActorAnimation>(Uuids::cameraActorAnimationUuid);
    ActorAnimationResRef camera1ActorAnimation = SafeGetResource<ActorAnimation>(Uuids::camera1ActorAnimationUuid);
    ActorAnimationResRef camera2ActorAnimation = SafeGetResource<ActorAnimation>(Uuids::camera2ActorAnimationUuid);
    ActorAnimationResRef lightActorAnimation = SafeGetResource<ActorAnimation>(Uuids::lightActorAnimationUuid);
    ActorAnimationResRef cubeActorAnimation = SafeGetResource<ActorAnimation>(Uuids::cubeActorAnimationUuid);

    // then
    ASSERT_TRUE(rootActor);
    ASSERT_TRUE(cameraActor);
    ASSERT_TRUE(camera1Actor);
    ASSERT_TRUE(camera2Actor);
    ASSERT_TRUE(lightActor);
    ASSERT_TRUE(cubeActor);

    ASSERT_TRUE(rootActorAnimation);
    ASSERT_TRUE(cameraActorAnimation);
    ASSERT_TRUE(camera1ActorAnimation);
    ASSERT_TRUE(camera2ActorAnimation);
    ASSERT_TRUE(lightActorAnimation);
    ASSERT_TRUE(cubeActorAnimation);

    ASSERT_EQ((*rootActorAnimation)->GetActor().Get(), rootActor.Get());
    ASSERT_EQ((*cameraActorAnimation)->GetActor().Get(), cameraActor.Get());
    ASSERT_EQ((*camera1ActorAnimation)->GetActor().Get(), camera1Actor.Get());
    ASSERT_EQ((*camera2ActorAnimation)->GetActor().Get(), camera2Actor.Get());
    ASSERT_EQ((*lightActorAnimation)->GetActor().Get(), lightActor.Get());
    ASSERT_EQ((*cubeActorAnimation)->GetActor().Get(), cubeActor.Get());

}


TEST_F(SchemaTest, DemoAnimation)
{
    // given
    BuildDemo();

    // when
    Ref<Resource<DemoAnimation>> demoAnimation = SafeGetResource<DemoAnimation>(Uuids::demoAnimationUuid);

    // then

    ASSERT_TRUE(demoAnimation);
    ASSERT_TRUE(*demoAnimation);

    ASSERT_EQ(1, m_demo->GetAnimationCount());
    ASSERT_EQ((*demoAnimation).Get(), m_demo->GetAnimation(0).Get());

    ASSERT_TRUE((*demoAnimation)->GetTarget());
    ASSERT_EQ((*demoAnimation)->GetTarget().Get(), m_demo.Get());
}
