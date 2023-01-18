// qnd loaderbar

#pragma once

#include "render/renderer.h"
#include "render/effect.h"

#include "resource/ResourceManager.h"

namespace GrafkitData {

	class LoaderBar : public Grafkit::IPreloadEvents {
	public:
		LoaderBar();
		virtual ~LoaderBar();

		void InitializeLoaderBar(Grafkit::Renderer & renderer);
		void ShutdownLoaderBar();

		// --- 
	    void OnElemLoad(size_t actual, size_t count) override;

	protected:
		// --- 
		// Dirty hack to obtain renderer object 
		// without hacking it through the resource loader
		// (unlike I saw doing it for real at some workplaces)
		virtual void UpdateLoaderBar(float p) = 0;
		void DrawLoaderBar(Grafkit::Renderer &m_render, float p);

		// To override ps if needed
		virtual std::string GetLoaderBarSrc();

	protected:
		// Abstract factory to substitude the single shader with other funky things
		virtual Grafkit::ShaderResRef CreateShader(Grafkit::Renderer & renderer);

	private:
		Grafkit::EffectComposerRef m_loaderbar;
		Grafkit::ShaderResRef m_fsLoader;
	};

}