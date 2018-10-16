#pragma once

#include "common.h"
#include "resource/ResourceBuilder.h"


class DummyObject
{
public:
    DummyObject() : m_param1(0)
        , m_param2(0)
        , m_isInitialized(false) {
    }

    DummyObject(float param1, float param2)
        : m_param1(param1)
        , m_param2(param2)
        , m_isInitialized(false) {
    }

    void Initialize() { m_isInitialized = true; }

    float GetParam1() const { return m_param1; }
    void SetParam1(const float param1) { m_param1 = param1; }
    float GetParam2() const { return m_param2; }
    void SetParam2(const float param2) { m_param2 = param2; }

    bool GetIsInitialized() const { return m_isInitialized; }

private:
    float m_param1;
    float m_param2;

    bool m_isInitialized;
};

struct DummyResourceParameters
{
    std::string sourceName;
    float param1;
    float param2;
};

class DummyObjectLoader : public Grafkit::ResourceBuilder<DummyObject, DummyResourceParameters>
{
public:
    void Load(Grafkit::IResourceManager* const& resman, Grafkit::IResource* source) override;
    void Initialize(Grafkit::Renderer& render, Grafkit::IResourceManager* const& resman, Grafkit::IResource* source) override;
};

inline void DummyObjectLoader::Load(Grafkit::IResourceManager* const& resman, Grafkit::IResource* source)
{
    Ref<Grafkit::Resource<DummyObject>> myResource = SafeGetResource(source);

    Ref<DummyObject> object = new DummyObject();

    object->SetParam1(m_params.param1);
    object->SetParam2(m_params.param2);

    myResource->AssingnRef(object);
}

inline void DummyObjectLoader::Initialize(Grafkit::Renderer& render, Grafkit::IResourceManager* const& resman, Grafkit::IResource* source)
{
    Ref<DummyObject> myObject = SafeGetObject(source);
    myObject->Initialize();
}
