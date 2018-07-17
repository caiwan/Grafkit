#pragma once

#include <string>

#include "utils/exceptions.h"

#include "utils/resource.h"
#include "utils/ResourceBuilder.h"
#include "utils/ResourceManager.h"

/**
Dummy class
*/

class Thing : public Referencable
{
public:
    Thing() : m_glejd(2.16f)
        , m_asd(0xfacababa) {
    }

    float GetGlejd() const { return m_glejd; }
    void SetGlejd(const float glejd) { m_glejd = glejd; }
    int GetAsd() const { return m_asd; }
    void SetAsd(const int asd) { m_asd = asd; }

private:
    float m_glejd;
    int m_asd;
};

typedef Ref<Thing> ThingRef;
typedef Grafkit::Resource<Thing> ThingResource;
typedef Ref<ThingResource> ThingResourceRef;

/**
Dummy class2
*/

class AnotherThing : public Referencable
{
public:
    AnotherThing() : m_sometingElse("It's completely different") {
    }

    std::string GetSometingElse() const { return m_sometingElse; }
    void SetSometingElse(const std::string& sometingElse) { m_sometingElse = sometingElse; }
private:
    std::string m_sometingElse;
};

typedef Ref<AnotherThing> AnotherThingRef;
typedef Grafkit::Resource<AnotherThing > AnotherThingResource;
typedef Ref<AnotherThingResource> AnotherThingResourceRef;

/** Dummy class builder
*/

class ThingLoader : public Grafkit::IResourceBuilder
{
public:
    ThingLoader(std::string name) : IResourceBuilder(name, name) {
    }

    virtual ~ThingLoader() {
    }

    Grafkit::IResource* NewResource() override { return new ThingResource(); }

    void Load(Grafkit::IResourceManager* const & resman, Grafkit::IResource* source) override
    {
        // 1.
        ThingResourceRef dstThing = dynamic_cast<ThingResource*>(source);
        if (dstThing.Invalid()) { return; }

        //if (dstThing->Valid()) { dstThing->Release(); }

        // 2.
        ThingRef thing = new Thing();
        thing->SetAsd(0x010101);

        // 3.
        dstThing->AssingnRef(thing);
    }
};

class ErrorousThingLoader : public Grafkit::IResourceBuilder
{
public:
    ErrorousThingLoader(std::string name) : IResourceBuilder(name, name) {
    }

    virtual ~ErrorousThingLoader() {
    }

    Grafkit::IResource* NewResource() override { return new ThingResource(); }

    void Load(Grafkit::IResourceManager* const & resman, Grafkit::IResource* source) override
    {
        // 1.
        ThingResourceRef dstThing = dynamic_cast<ThingResource*>(source);
        if (dstThing.Invalid()) { return; }

        // 2.
        ThingRef thing = new Thing();

        // oops, something happened intentionally 
        throw new EX(FWdebugExceptions::NullPointerException);
    }
};

/** Dummy resouece manager class
*/

class MyResourceManager : public Grafkit::IResourceManager
{
public:

    MyResourceManager() {
    }

    ~MyResourceManager() {
    }

    Grafkit::IAssetFactory* GetAssetFactory() override { return nullptr; }

    Grafkit::Renderer& GetDeviceContext() override
    {
        static Grafkit::Renderer render;
        return render;
    }
};
