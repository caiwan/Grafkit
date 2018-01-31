
#pragma once 

#include "render/effect.h"

#include "demo/sceneelem.h"
#include "demo/demoframework.h"

namespace Grafkit{
	class DofEffect : public GKDemo::SceneElem
	{

	public:
		DofEffect();
		~DofEffect();

	public:
		virtual void OnBeforePreload(Grafkit::Renderer &render, GKDemo::DemoApplication *const& context, Grafkit::IResourceManager * const & resman) = 0;
		virtual void OnAfterPreload(Grafkit::Renderer &render, GKDemo::DemoApplication *const& context) = 0;
		virtual void OnDelegateTracks(Ref<Grafkit::ValueTracker> &tracker, GKDemo::DemoApplication *const& context) {}
		virtual void OnSetupResources(Grafkit::Renderer &render, GKDemo::DemoApplication *const& context) {}
		virtual void OnBeforeRender(Grafkit::Renderer &render, GKDemo::DemoApplication *const& context) {}
		virtual void OnRender(Grafkit::Renderer &render, GKDemo::DemoApplication *const& context) = 0;
		virtual void OnAfterRender(Grafkit::Renderer &render, GKDemo::DemoApplication *const& context) {}

	private:
		Grafkit::EffectComposerRef fxPrecalc;
		Grafkit::EffectComposerRef fxPass[3];
		Grafkit::EffectComposerRef fxCombine;

		Grafkit::ShaderResRef fsPrecalc;
		Grafkit::ShaderResRef fsBlur;
		Grafkit::ShaderResRef fsCombine;

		Grafkit::TextureRef dofAmountMap;
		Grafkit::TextureRef blurPassOut[3];

		struct {
			float Aperture;
			float Focus;
			float angle;
		} blurParam[6];

		struct {
			//https://en.wikipedia.org/wiki/Circle_of_confusion
			float Aperture;
			float Focus; // multiplied with distanceUnit (see above)
			float Limit; // blur limit, screen space
		} cocParam;

	};

}


