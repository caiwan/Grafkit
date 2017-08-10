#pragma once 

#include <vector>

#include "../utils/reference.h"
#include "../utils/memory.h"

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
		virtual ~EffectComposer();

		void Initialize(Renderer &render);
		void Shutdown();

		void AddPass(EffectPassRef in_fx) { m_effectChain.push_back(in_fx);}

		EffectPassRef GetEffect(size_t id) { return id <m_effectChain.size()? m_effectChain[id] : EffectPassRef(); }

		/** Beallitja az elso read buffert a rendererenek, a render passhoz
		*/
		void BindInput(Renderer &render);
		void UnbindInput(Renderer& render);

		/** Vegigmegy a render chainen
		*/
		void Render(Renderer &render, int autoflush = 1);
		void Flush(Renderer &render);

		void SetInput(size_t bind, TextureRef tex) { m_input_map[bind] = tex; }
		TextureRef GetInput(size_t bind) { auto it = m_input_map.find(bind); return it == m_input_map.end() ? TextureRef() : it->second; }
		void ClearInput() { m_input_map.clear(); }

	protected:
		/**
		Megcsereli a read es a write buffert
		*/
		void SwapBuffers();

		/**
		Megcsereli az utolso read buffert a backbufferrel
		*/
		void FlushBuffers();

		/**
		Lerendereli a chaint
		*/
		void RenderChain(Renderer &render);

	protected:

		TextureRef m_texOut[3];
		Texture2D *m_pTexRead, *m_pTexWrite, *m_pTexBack;

		TextureSamplerRef m_textureSampler;

		MeshRef m_fullscreenquad;
		ShaderRef m_shaderFullscreenQuad;
		ShaderRef m_shaderCopyScreen;

		std::vector<EffectPassRef> m_effectChain;
		typedef std::vector<EffectPassRef>::iterator fx_chain_it_t;

		typedef std::map<size_t, TextureRef> bind_map_t;
		typedef bind_map_t::iterator bind_map_it_t;

		bind_map_t m_input_map;
	};

	// ---------------------------------------------------------------------------------------------------

	class EffectComposerRef : public EffectComposerRef_t {
	public:
		EffectComposerRef() : EffectComposerRef_t() {}
		EffectComposerRef(EffectComposerRef &other) : EffectComposerRef_t(other) {}
		EffectComposerRef(EffectComposer *other) : EffectComposerRef_t(other) {}

		EffectPassRef operator [] (int id) { return Valid() ? this->ptr->GetEffect(id) : EffectPassRef(); }
	};

	// ===================================================================================================

	__declspec(align(16)) class EffectPass : virtual public Referencable, public AlignedNew<EffectPass>
	{
		friend class EffectComposer;
	public :
		EffectPass(ShaderResRef shader);
		~EffectPass();

		void Initialize(Renderer &render);
		void Shutdown();

		size_t BindOutputs(Renderer &render);
		size_t UnbindOutputs(Renderer &render);

		void BindFx(Renderer &render);
		void UnbindFx(Renderer &render);

		ShaderRef GetShader() { return m_shader->Get(); }

		void SetOutput(size_t bind, TextureRef tex) {m_output_map[bind] = tex; }
		void SetInput(std::string name, TextureRef tex) { m_input_map[name] = tex; }

		void ClearOutputs() { m_output_map.clear();  }
		void ClearInputs() { m_input_map.clear(); }

		TextureRef GetOutput(size_t bind); 
		TextureRef GetInput(std::string name);

	private:
		ShaderResRef m_shader;

		typedef std::map<size_t, TextureRef> bind_map_t;
		typedef bind_map_t::iterator bind_map_it_t;
		typedef std::map<std::string, TextureRef> name_map_t;
		typedef name_map_t::iterator name_map_it_t;

		bind_map_t m_output_map;
		name_map_t m_input_map;

	};

	// ---------------------------------------------------------------------------------------------------
	
	// ===================================================================================================

}
