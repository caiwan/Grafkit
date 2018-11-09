#include "stdafx.h"

#include "utils/asset/AssetFile.h"

#include "scene/scene.h"

#include "core/system.h"

#include "render/material.h"
#include "render/model.h"
#include "render/texture.h"

// --- 

using namespace GkDemo;
using namespace Grafkit;

#define JSON_TERXTURE_PATH "test_texture.json"
#define JSON_MATERIAL_PATH "test_material.json"

class LoadMaterialTest : public testing::Test
{
public:
    LoadMaterialTest() {
    }

    virtual ~LoadMaterialTest() {
    }

    void SetUp() override
    {
        m_app = std::make_unique<Testing::TestApplicationContext>();
        m_app->Initialize();
    }

    void TearDown() override { m_app->Release(); }

protected:
    std::unique_ptr<Testing::TestApplicationContext> m_app;
};


// ============================================================================================

template <class T>
class TextureWrapper
{
    typedef T my_type;
    Ref<Resource<T>> resource;

public:
    explicit TextureWrapper(const Ref<Resource<T>>& resource)
        : resource(resource) {
    }

    constexpr void validate(const char* name, const char* uuid) const
    {
        ASSERT_TRUE(resource);
        ASSERT_TRUE((*resource));
        ASSERT_STREQ(uuid, resource->GetUuid().c_str());
        ASSERT_STREQ(name, resource->GetName().c_str());

        ASSERT_TRUE((*resource)->GetTexture2D()) << name << " " << uuid;
        ASSERT_TRUE((*resource)->GetTextureResource()) << name << " " << uuid;
        ASSERT_TRUE((*resource)->GetShaderResourceView()) << name << " " << uuid;
    }
};

TEST_F(LoadMaterialTest, TextureLoad)
{
    // given: context
    m_app->GetContext().LoadDemo(JSON_TERXTURE_PATH);
    m_app->GetContext().DoPrecalc();

    // when
    TextureWrapper<Texture2D> normapTexture(m_app->SafeGetResource<Texture2D>(Uuids::normapUuid));
    TextureWrapper<Texture2D> checkerBoardTexture(m_app->SafeGetResource<Texture2D>(Uuids::checkerBoardUuid));
    TextureWrapper<TextureCube> skyTextureTexture(m_app->SafeGetResource<TextureCube>(Uuids::skyTextureUuid));
    TextureWrapper<TextureCube> skyRadianceTextureTexture(m_app->SafeGetResource<TextureCube>(Uuids::skyRadianceTextureUuid));
    TextureWrapper<Texture2D> noise256TextureTexture(m_app->SafeGetResource<Texture2D>(Uuids::noise256TextureUuid));
    TextureWrapper<Texture2D> noise512TextureTexture(m_app->SafeGetResource<Texture2D>(Uuids::noise512TextureUuid));

    // then

    // bitmap
    normapTexture.validate("normap", Uuids::normapUuid);
    checkerBoardTexture.validate("UVAddressBoard", Uuids::checkerBoardUuid);

    // cubemap
    skyTextureTexture.validate("Sky1", Uuids::skyTextureUuid);
    skyRadianceTextureTexture.validate("SkyRadiance", Uuids::skyRadianceTextureUuid);
    
    // noise kernel
    noise256TextureTexture.validate("Noise256", Uuids::noise256TextureUuid);
    noise512TextureTexture.validate("Noise512", Uuids::noise512TextureUuid);
}

#if 0


namespace Uuids
{
    const char * normapMaterialUuid = "1c982eed-494f-4648-96fc-29f6e33becd9";
    const char * checkerMaterialUuid = "74e35451-384d-4ae9-b4bf-d630f4cebf63";
    const char * torusPbrMaterialUuid = "24dc43f2-4bb8-46a1-8614-ead866237cea";
}

TEST_F(ContextMaterialTest, MaterialLoad)
{
    // given: context
    this->BuildDemo();
    ASSERT_TRUE(m_demo.Valid());

    // when
    MaterialRef normapMaterial = SafeGetObject<Material>(Uuids::normapMaterialUuid);
    MaterialRef checkerBoardMaterial = SafeGetObject<Material>(Uuids::checkerMaterialUuid);

    ASSERT_TRUE(normapMaterial);
    ASSERT_TRUE(checkerBoardMaterial);

    // then
    ASSERT_STREQ(Uuids::normapMaterialUuid, normapMaterial->GetUuid().c_str());
    ASSERT_STREQ("NormapMaterial", normapMaterial->GetName().c_str());

    ASSERT_STREQ(Uuids::checkerMaterialUuid, checkerBoardMaterial->GetUuid().c_str());
    ASSERT_STREQ("CheckerBoardMaterial", checkerBoardMaterial->GetName().c_str());
}


TEST_F(ContextMaterialTest, TextureBoundMaterialLoad)
{
    // given: context
    this->BuildDemo();
    ASSERT_TRUE(m_demo.Valid());

    // when
    MaterialRef normapMaterial = SafeGetObject<Material>(Uuids::normapMaterialUuid);
    MaterialRef checkerBoardMaterial = SafeGetObject<Material>(Uuids::checkerMaterialUuid);
    MaterialRef torusPbrMaterial = SafeGetObject<Material>(Uuids::torusPbrMaterialUuid);

    ASSERT_TRUE(normapMaterial);
    ASSERT_TRUE(checkerBoardMaterial);
    ASSERT_TRUE(torusPbrMaterial);

    // then
    ASSERT_TRUE(normapMaterial->GetTexture("diffuse"));
    ASSERT_STREQ(Uuids::normapUuid, normapMaterial->GetTexture("diffuse")->GetName().c_str());
    ASSERT_STREQ("normap", normapMaterial->GetTexture("diffuse")->GetUuid().c_str());

    ASSERT_TRUE(checkerBoardMaterial->GetTexture("diffuse"));
    ASSERT_STREQ(Uuids::checkerBoardUuid, checkerBoardMaterial->GetTexture("diffuse")->GetName().c_str());
    ASSERT_STREQ("UVAddressBoard", checkerBoardMaterial->GetTexture("diffuse")->GetUuid().c_str());

    /*ASSERT_TRUE(torusPbrMaterial->GetTexture("diffuse"));
    ASSERT_STREQ(Uuids::torusPbrMaterialUuid, torusPbrMaterial->GetTexture("diffuse")->GetName().c_str());
    ASSERT_STREQ("TorusPbrMaterial", torusPbrMaterial->GetTexture("diffuse")->GetUuid().c_str());*/
}

#endif
