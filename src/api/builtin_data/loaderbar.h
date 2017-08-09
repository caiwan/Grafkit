// qnd loaderbar

#pragma once

#include "render/renderer.h"
#include "render/effect.h"

#include "utils/ResourceManager.h"

namespace GrafkitData {

	class LoaderBar : public Grafkit::IPreloadEvents {
	public:
		LoaderBar();
		~LoaderBar();
		void InitializeLoaderBar(Grafkit::Renderer & renderer);
		void ShutdownLoaderBar();

		// --- 
		//virtual void OnBeginLoad() = 0;
		//virtual void OnBeforeElemLoad(IResourceBuilder *&builder, IResource *&res) = 0;
		virtual void OnElemLoad(size_t actual, size_t count) ;
		//virtual void OnEndLoad() = 0;

	protected:
		// --- 
		// Dirty hack to obtain renderer object 
		// without hacking it through the resource loader
		// (unlike I saw doing it for real at some workplaces)
		virtual void UpdateLoaderBar(float p) = 0;
		void DrawLoaderBar(Grafkit::Renderer &render, float p);

	protected:
		// Abstract factory to substitude the single shader with other funky things
		virtual Grafkit::ShaderResRef CreateShader(Grafkit::Renderer & renderer);

	protected:
		Grafkit::EffectComposerRef m_loaderbar;
		Grafkit::ShaderResRef m_ps;
	};

}