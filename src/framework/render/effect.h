#pragma once 

#include <vector>

#include "../utils/reference.h"
#include "../utils/memory.h"

#include "renderer.h"
#include "texture.h"
#include "shader.h"
#include "mesh.h"

#include "shaderparameter.h"

namespace Grafkit {
	
	class ShaderParameter;

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

		void Initialize(Renderer &render, bool singlepass = false);
		void Shutdown();

		void AddPass(EffectPassRef pass) { m_effectChain.push_back(pass);}

		EffectPassRef GetEffect(size_t id) { return id <m_effectChain.size()? m_effectChain[id] : EffectPassRef(); }

		void BindInput(Renderer &render);
		void UnbindInput(Renderer& render);

		void Render(Renderer &render, int autoflush = 1);
		void Flush(Renderer &render);

		void SetInput(size_t bind, TextureRef tex) { m_input_map[bind] = tex; }
		TextureRef GetInput(size_t bind) { auto it = m_input_map.find(bind); return it == m_input_map.end() ? TextureRef() : it->second; }
		void ClearInput() { m_input_map.clear(); }

		Texture2DRef GetOutput() { return m_pTexLastOutput; }

	protected:
		void SwapBuffers();
		void FlushBuffers();
		void RenderChain(Renderer &render);

	protected:

		std::vector<Ref<EffectComposer>> m_parentSource;

		TextureRef m_texOut[4];
		Texture2D *m_pTexRead, *m_pTexWrite, *m_pTexFront, *m_pTexBack;
		Texture2D *m_pTexLastOutput; 

		TextureSamplerRef m_textureSampler;

		MeshRef m_fullscreenquad;
		ShaderRef m_shaderFullscreenQuad;
		ShaderRef m_shaderCopyScreen;

		std::vector<EffectPassRef> m_effectChain;
		typedef std::map<size_t, TextureRef> bind_map_t;
		
		bind_map_t m_input_map;

		struct {
			float4 s, v;
		} m_screen_params;

		bool m_singlepass;
	};

	// ---------------------------------------------------------------------------------------------------

#if 1
	typedef EffectComposerRef_t EffectComposerRef;
#else 
	class EffectComposerRef : public EffectComposerRef_t {
	public:
		EffectComposerRef() : EffectComposerRef_t() {}
		EffectComposerRef(EffectComposerRef &other) : EffectComposerRef_t(other) {}
		EffectComposerRef(EffectComposer *other) : EffectComposerRef_t(other) {}

		EffectPassRef operator [] (int id) { return Valid() ? this->ptr->GetEffect(id) : EffectPassRef(); }
	};
#endif 
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

		Ref<IRenderElement> GetParameter() { return m_shaderParameter; }

	private:
		ShaderResRef m_shader;

		Ref<IRenderElement> m_shaderParameter;

		typedef std::map<size_t, TextureRef> bind_map_t;
		typedef bind_map_t::iterator bind_map_it_t;
		typedef std::map<std::string, TextureRef> name_map_t;
		typedef name_map_t::iterator name_map_it_t;

		bind_map_t m_output_map;
		name_map_t m_input_map;

	};

	// ===================================================================================================

}
