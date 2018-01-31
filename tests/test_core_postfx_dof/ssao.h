#pragma once

#include "render/effect.h"

#include "demo/sceneelem.h"
#include "demo/demoframework.h"

class SsaoEffect : public GKDemo::SceneElem
{
public :
	SsaoEffect();
	~SsaoEffect();

public:
	virtual void OnBeforePreload(Grafkit::Renderer &render, GKDemo::DemoApplication *const& context, Grafkit::IResourceManager * const & resman) = 0;
	virtual void OnAfterPreload(Grafkit::Renderer &render, GKDemo::DemoApplication *const& context) = 0;
	virtual void OnDelegateTracks(Ref<Grafkit::ValueTracker> &tracker, GKDemo::DemoApplication *const& context) {}
	virtual void OnSetupResources(Grafkit::Renderer &render, GKDemo::DemoApplication *const& context) {}
	virtual void OnBeforeRender(Grafkit::Renderer &render, GKDemo::DemoApplication *const& context) {}
	virtual void OnRender(Grafkit::Renderer &render, GKDemo::DemoApplication *const& context) = 0;
	virtual void OnAfterRender(Grafkit::Renderer &render, GKDemo::DemoApplication *const& context) {}

private:
	Grafkit::ShaderResRef aoFs, blurFs;
};