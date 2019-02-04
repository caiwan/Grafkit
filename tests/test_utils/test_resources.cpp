#include <cassert>
#include <gtest/gtest.h>

#include "render/renderer.h"
#include "core/resource.h"
#include "core/asset.h"
#include "resource/ResourceManager.h"
#include "resource/ResourceBuilder.h"
#include "utils/asset/AssetFactory.h"

using namespace Grafkit;

class DummyAssetFactory : public IAssetFactory
{
public:
    void SetBasePath(const std::string& path) override
    {
        assert(0);
    }

    StreamRef Get(std::string name) const override
    {
        assert(0);
        return {};
    };
};

// --- 
// These are the resources we want to load 
class TestResouceA
{
public:
    int m_a, m_b;
};

class TestResouceB
{
public:
    int m_c;
    double m_d;
};

class TestBaseResource
{
public:
    int m_a, m_b;
    virtual void SomePolymorhpicTrait() = 0;
};

class TestDerivedResourceA : public TestBaseResource
{
public:
    Resource<TestResouceA> m_dependA;
    int m_c;
    double m_d;

    void SomePolymorhpicTrait() override {
    };
};

class TestDerivedResourceB : public TestBaseResource
{
public:
    Resource<TestResouceB> m_dependB;
    std::string m_something;

    void SomePolymorhpicTrait() override {
    };
};

// ---
// These are the generators for each resources
struct TestResouceAParams
{
    int a, b;
};

class TestResouceALoader : public ResourceLoader<TestResouceA, TestResouceAParams>
{
public:
    TestResouceALoader(Uuid id, TestResouceAParams params)
        : ResourceLoader<TestResouceA, TestResouceAParams>(id, params) {
    }

    void Load(const std::shared_ptr<IResource>& resource, ResourceManager&, const IAssetFactory&) override
    {
        auto newResource = std::make_shared<TestResouceA>();

        if (m_params.a == 42)
            throw std::runtime_error("Invalid param a");

        newResource->m_a = m_params.a;
        newResource->m_b = m_params.b;
        auto myResource = CastResource(resource);
        myResource.AssignRef(newResource);
    }

    void Initialize(const std::shared_ptr<IResource>& resource, ResourceManager&, const IAssetFactory&) override {
    }
};

class TestResouceBLoader : public ResourceLoader<TestResouceB>
{
public:

    explicit TestResouceBLoader(const Uuid& id, const None& params = {})
        : ResourceLoader<TestResouceB>(id, params) {
    }

    void Load(const std::shared_ptr<IResource>& resource, ResourceManager&, const IAssetFactory&) override
    {
        auto myResource = CastResource(resource);
        myResource.AssignRef(std::make_shared<TestResouceB>());
        myResource->m_c = 42;
        myResource->m_d = 2.16;
    }

    void Initialize(const std::shared_ptr<IResource>& resource, ResourceManager&, const IAssetFactory&) override {
    }
};

struct TestDerivedResourceAParams
{
    int a, b, c, d;
    Uuid resourceAId;
};

class TestDerivedResourceALoader : public ResourceLoader<TestDerivedResourceA, TestDerivedResourceAParams>
{
public:

    TestDerivedResourceALoader(const Uuid& id, const TestDerivedResourceAParams& params)
        : ResourceLoader<TestDerivedResourceA, TestDerivedResourceAParams>(id, params) {
    }

    void Load(const std::shared_ptr<IResource>& resource, ResourceManager& resourceManager, const IAssetFactory&) override
    {
        auto newResource = std::make_shared<TestDerivedResourceA>();

        // For error testing purpuses
        if (m_params.a != 42)
            throw std::runtime_error("Invalid param a");

        newResource->m_a = m_params.a;
        newResource->m_b = m_params.b;
        newResource->m_c = m_params.c;
        newResource->m_d = m_params.d;

        auto dependingResource = resourceManager.Get<TestResouceA>(m_params.resourceAId);
        newResource->m_dependA = dependingResource;

        // It's the loaders responsibility to keep the integrity upon error 
        auto myResource = CastResource(resource);
        myResource.AssignRef(newResource);
    }

    void Initialize(const std::shared_ptr<IResource>&, ResourceManager&, const IAssetFactory&) override {
    }
};

struct TestDerivedResourceBParams
{
    int a, b;
    std::string something;
    Uuid resourceBId;
};

class TestDerivedResourceBLoader : public ResourceLoader<TestDerivedResourceB, TestDerivedResourceBParams>
{
public:

    TestDerivedResourceBLoader(const Uuid& id, const TestDerivedResourceBParams& params)
        : ResourceLoader<TestDerivedResourceB, TestDerivedResourceBParams>(id, params) {
    }

    void Load(const std::shared_ptr<IResource>& resource, ResourceManager& resourceManager, const IAssetFactory& assetFactory) override
    {
        auto myResource = CastResource(resource);
        myResource.AssignRef(std::make_shared<TestDerivedResourceB>());
        myResource->m_a = m_params.a;
        myResource->m_b = m_params.b;
        myResource->m_something = m_params.something;

        auto dependingResource = resourceManager.Get<TestResouceB>(m_params.resourceBId);
        myResource->m_dependB = dependingResource;
    }

    void Initialize(const std::shared_ptr<IResource>& resource, ResourceManager& resourceManager, const IAssetFactory&) override {
    }
};


TEST(Resource, LoadObjectTest)
{
    // Given
    Renderer render;
    DummyAssetFactory assetFactory;
    auto manager = std::make_unique<ResourceManager>();

    auto resourceA = manager->Load<TestResouceA>(std::make_shared<TestResouceALoader>(Uuid("46a44293-aaa3-47f0-b6e5-82abd9ba72c0"), TestResouceAParams{1,2}));
    auto resourceB = manager->Load<TestResouceB>(std::make_shared<TestResouceBLoader>(Uuid("000273ce-bef5-44a9-bf30-d594de396212")));

    // When
    manager->DoPrecalc(render, assetFactory);

    // Then
    ASSERT_TRUE(*resourceA);
    ASSERT_TRUE(1 == resourceA->m_a);
    ASSERT_TRUE(2 == resourceA->m_b);

    ASSERT_TRUE(*resourceB);
    ASSERT_TRUE(42 == resourceB->m_c);
    ASSERT_TRUE(2.16 == resourceB->m_d);
}

TEST(Resource, InheritionAndDependemncyTest)
{
    // Given
    Renderer render;
    DummyAssetFactory assetFactory;

    auto manager = std::make_unique<ResourceManager>();

    auto resourceA = manager->Load<TestResouceA>(std::make_shared<TestResouceALoader>(Uuid("46a44293-aaa3-47f0-b6e5-82abd9ba72c0"), TestResouceAParams{3,4}));
    auto resourceB = manager->Load<TestResouceB>(std::make_shared<TestResouceBLoader>(Uuid("000273ce-bef5-44a9-bf30-d594de396212")));

    auto resourceDA = manager->Load<TestDerivedResourceA>(std::make_shared<TestDerivedResourceALoader>(Uuid("3a38e7d1-4971-444a-b840-5bb2bbe09a44"), TestDerivedResourceAParams{42,2,3,4, Uuid("46a44293-aaa3-47f0-b6e5-82abd9ba72c0")}));
    auto resourceDB = manager->Load<TestDerivedResourceB>(std::make_shared<TestDerivedResourceBLoader>(Uuid("35747964-082b-45d5-a367-cc38042e8150"), TestDerivedResourceBParams{1,2,"Something", Uuid("000273ce-bef5-44a9-bf30-d594de396212")}));

    // When
    manager->DoPrecalc(render, assetFactory);

    // Then
    ASSERT_TRUE(resourceA.Valid());
    ASSERT_TRUE(resourceB.Valid());

    ASSERT_TRUE(resourceDA.Valid());
    ASSERT_TRUE(42 == resourceDA->m_a);
    ASSERT_TRUE(2 == resourceDA->m_b);
    ASSERT_TRUE(3 == resourceDA->m_c);
    ASSERT_TRUE(4 == resourceDA->m_d);
    ASSERT_TRUE(*resourceA == *resourceDA->m_dependA);

    ASSERT_TRUE(resourceDB.Valid());
    ASSERT_TRUE(0 == std::string("Something").compare(resourceDB->m_something));
    ASSERT_TRUE(*resourceB == *resourceDB->m_dependB);

    // --- Casting 
    Resource<TestBaseResource> baseA = manager->Get<TestBaseResource>(Uuid("3a38e7d1-4971-444a-b840-5bb2bbe09a44"));
    Resource<TestBaseResource> baseB = manager->Get<TestBaseResource>(Uuid("35747964-082b-45d5-a367-cc38042e8150"));

    ASSERT_TRUE(baseA);
    ASSERT_TRUE(baseB);

    Resource<TestDerivedResourceA> objA = baseA.CastTo<TestDerivedResourceA>();
    Resource<TestDerivedResourceB> objB = baseB.CastTo<TestDerivedResourceB>();

    ASSERT_TRUE(objA);
    ASSERT_TRUE(objB);
}

TEST(Resource, LoadWithErrorTest)
{
    // Given
    Renderer render;
    DummyAssetFactory assetFactory;

    auto manager = std::make_unique<ResourceManager>();
    auto resourceDA = manager->Load<TestDerivedResourceA>(std::make_shared<TestDerivedResourceALoader>(Uuid("3a38e7d1-4971-444a-b840-5bb2bbe09a44"), TestDerivedResourceAParams{1,2,3,4,Uuid()}));

    // When
    try
    {
        manager->DoPrecalc(render, assetFactory);
        FAIL();
    }
    catch (std::exception& e) {
    }

    // Then
    ASSERT_TRUE(!resourceDA.Empty());
    ASSERT_TRUE(!resourceDA);
}

TEST(Resource, ReloadTest)
{
    // Given
    Renderer render;
    DummyAssetFactory assetFactory;

    auto manager = std::make_unique<ResourceManager>();

    auto resourceA = manager->Load<TestResouceA>(std::make_shared<TestResouceALoader>(Uuid("46a44293-aaa3-47f0-b6e5-82abd9ba72c0"), TestResouceAParams{1,2}));

    manager->DoPrecalc(render, assetFactory);

    ASSERT_TRUE(*resourceA);
    ASSERT_TRUE(1 == resourceA->m_a);
    ASSERT_TRUE(2 == resourceA->m_b);

    // When
    manager->GetLoader<TestResouceALoader>(Uuid("46a44293-aaa3-47f0-b6e5-82abd9ba72c0"))->SetParams(TestResouceAParams{3,4});
    manager->Reload(Uuid("46a44293-aaa3-47f0-b6e5-82abd9ba72c0"));

    manager->DoPrecalc(render, assetFactory);

    // Then
    ASSERT_TRUE(*resourceA);
    ASSERT_TRUE(3 == resourceA->m_a);
    ASSERT_TRUE(4 == resourceA->m_b);
}

TEST(Resource, ReloadWithErrorTest)
{
    // Given
    Renderer render;
    DummyAssetFactory assetFactory;
    auto manager = std::make_unique<ResourceManager>();

    auto resourceA = manager->Load<TestResouceA>(std::make_shared<TestResouceALoader>(Uuid("46a44293-aaa3-47f0-b6e5-82abd9ba72c0"), TestResouceAParams{1,2}));

    // When
    manager->DoPrecalc(render, assetFactory);

    ASSERT_TRUE(*resourceA);
    ASSERT_TRUE(1 == resourceA->m_a);
    ASSERT_TRUE(2 == resourceA->m_b);

    manager->GetLoader<TestResouceALoader>(Uuid("46a44293-aaa3-47f0-b6e5-82abd9ba72c0"))->SetParams(TestResouceAParams{42, 2});
    manager->Reload(Uuid("46a44293-aaa3-47f0-b6e5-82abd9ba72c0"));

    // Then
    try
    {
        manager->DoPrecalc(render, assetFactory);
        FAIL();
    }
    catch (std::exception& e) {
    }
    // Should not be replaced
    ASSERT_TRUE(resourceA.Valid());
    ASSERT_TRUE(1 == resourceA->m_a);
    ASSERT_TRUE(2 == resourceA->m_b);
}

// 6 Casting and assigning doesn't hurt integrity.
TEST(Resource, CastingIntegrityTest)
{
    auto manager = std::make_unique<ResourceManager>();
    auto ra = Resource<TestResouceA>(std::make_shared<TestResouceA>());
    FAIL("Not implemented");
}