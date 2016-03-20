#pragma once 

#include <vector>

#include "reference.h"
#include "memory_align.h"

#include "renderer.h"
#include "texture.h"
#include "shader.h"
#include "mesh.h"

namespace Grafkit {
	
	class EffectComposer;
	class EffectPass;

	typedef Ref<EffectComposer> EffectComposerRef_t;
	typedef Ref<EffectPass> EffectPassRef;

	// ---------------------------------------------------------------------------------------------------

	__declspec(align(16)) class EffectComposer : virtual public Referencable, public AlignedNew<EffectComposer>
	{
	public:
		EffectComposer();
		~EffectComposer();

		void Initialize(Renderer &render);
		void Shutdown();

		void AddPass(EffectPassRef in_fx) { m_effectChain.push_back(in_fx); }

		EffectPassRef GetEffect(size_t id) { return id <m_effectChain.size()? m_effectChain[id] : EffectPassRef(); }

		/** Beallitja az elso read buffert a rendererenek, a render passhoz
		*/
		void BindInput(Renderer &render);

		/** Vegigmegy a render chainen
		*/
		void Render(Renderer &render);

	protected:
		/**
		Megcsereli a read es a write buffert
		*/
		void SwapBuffers();

		/**
		Megcsereli az utolso read buffert a backbufferrel
		*/
		void FlushBuffers();

	private:

		TextureRef m_texOut[3];
		Texture *m_pTexRead, *m_pTexWrite, *m_pTexBack;

		TextureSamplerRef m_textureSampler;

		MeshRef m_fullscreenquad;
		ShaderRef m_shaderFullscreenQuad, m_shaderCopyScreen;

		std::vector<EffectPassRef> m_effectChain;
		typedef std::vector<EffectPassRef>::iterator fx_chain_it_t;
	};

	// ---------------------------------------------------------------------------------------------------

	class EffectComposerRef : public EffectComposerRef_t {
	public:
		EffectComposerRef() : EffectComposerRef_t() {}
		EffectComposerRef(EffectComposerRef &other) : EffectComposerRef_t(other) {}
		EffectComposerRef(EffectComposer *other) : EffectComposerRef_t(other) {}

		EffectPassRef operator [] (size_t id) { return Valid() ? this->ptr->GetEffect(id) : EffectPassRef(); }
	};

	// ===================================================================================================

	__declspec(align(16)) class EffectPass : virtual public Referencable, public AlignedNew<EffectPass>
	{
		friend class EffectComposer;
	public :
		EffectPass();
		~EffectPass();

		void Initialize(Renderer &render, ShaderRef shader);
		void Shutdown();

		void Render(Renderer &render);

		ShaderRef GetShader() { return m_shader; }

	private:
		ShaderRef m_shader;

	};

	// ---------------------------------------------------------------------------------------------------
	
	// ===================================================================================================

}
