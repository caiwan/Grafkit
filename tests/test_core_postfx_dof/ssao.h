#pragma once

#include "render/effect.h"
#include "render/textue.h"

#include "demo/sceneelem.h"
#include "demo/demoframework.h"

class SsaoEffect : public GKDemo::SceneElem
{
public:
	SsaoEffect();
	~SsaoEffect();

	void Shutdown();

public:
	virtual void OnBeforePreload(Grafkit::Renderer &render, GKDemo::DemoApplication *const& context, Grafkit::IResourceManager * const & resman) = 0;
	virtual void OnAfterPreload(Grafkit::Renderer &render, GKDemo::DemoApplication *const& context) = 0;

	virtual void OnBeforeRender(Grafkit::Renderer &render, GKDemo::DemoApplication *const& context) {}
	virtual void OnRender(Grafkit::Renderer &render, GKDemo::DemoApplication *const& context) = 0;
	virtual void OnAfterRender(Grafkit::Renderer &render, GKDemo::DemoApplication *const& context) {}

private:

	Grafkit::EffectComposerRef effect;

	Grafkit::ShaderResRef fsAo, fsBlur;
	Grafkit::TextureSamplerRef texSampler;

	float4 *kernels;

	TextureResRef texNoiseMap;

	Grafkit::TextureResRef inputMap;
	Grafkit::TextureResRef outputMap;

public:
	struct {
		float2 noiseScele;
		float radius;
		float treshold;
	}parameters;

};