#pragma once

#include <string>

#include "utils/exceptions.h"

#include "utils/resource.h"
#include "utils/ResourceBuilder.h"
#include "utils/ResourceManager.h"

/**
Dummy class
*/

class Thing : public Referencable {
private:
	float m_glejd;
	int m_asd;
public:
	Thing() : m_glejd(2.16f), m_asd(0xfacababa) {}
	float GetGlejd() { return m_glejd; }
	
	int GetAsd() { return m_asd; }
	void  SetAsd(int asd) { m_asd = asd; }
};

typedef Ref<Thing> ThingRef;

typedef Grafkit::Resource<Thing> ThingResource;

typedef Ref<ThingResource> ThingResourceRef;

/** Dummy class builder 
*/

class ThingLoader : public Grafkit::IResourceBuilder {
public:
	ThingLoader(std::string name) : Grafkit::IResourceBuilder(name, name) {}
	virtual ~ThingLoader() {}

	virtual Grafkit::IResource* NewResource() {
		return new ThingResource();
	}

	virtual void Load(Grafkit::IResourceManager * const & resman, Grafkit::IResource * source) {
		// 1.
		ThingResourceRef dstThing = dynamic_cast<ThingResource*>(source);
		if (dstThing.Invalid()){
			return;
		}

		if (dstThing->Valid()) {
			dstThing->Release();
		}

		// 2.
		ThingRef thing = new Thing();
		thing->SetAsd(0x010101);

		// 3.
		dstThing->AssingnRef(thing);

	}
};

class ErrorousThingLoader : public Grafkit::IResourceBuilder {
public:
	ErrorousThingLoader(std::string name) : Grafkit::IResourceBuilder(name, name) {}
	virtual ~ErrorousThingLoader() {}

	virtual Grafkit::IResource* NewResource() {
		return new ThingResource();
	}

	virtual void Load(Grafkit::IResourceManager * const & resman, Grafkit::IResource * source) {
		// 1.
		ThingResourceRef dstThing = dynamic_cast<ThingResource*>(source);
		if (dstThing.Invalid()) {
			return;
		}

		// 2.
		ThingRef thing = new Thing();
		
		// oops, something happened intentionally 
		throw new EX(FWdebugExceptions::NullPointerException); 
		
		thing->SetAsd(0x010101);

		// 3.
		if (dstThing->Valid()) {
			dstThing->Release();
		}
		dstThing->AssingnRef(thing);

	}
};

/** Dummy resouece manager class 
*/

class MyResourceManager : public Grafkit::IResourceManager {
public:

	MyResourceManager() {}
	~MyResourceManager() {}

	virtual Grafkit::IAssetFactory *GetAssetFactory() {
		return nullptr;
	}

	virtual Grafkit::Renderer &GetDeviceContext() {
		static Grafkit::Renderer render;
		return render;
	}
};
