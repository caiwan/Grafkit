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

		EffectPassRef GetPass(size_t id) { return id <m_effectChain.size()? m_effectChain[id] : EffectPassRef(); }

		void BindInput(Renderer &render);
		void UnbindInput(Renderer& render);

		void Render(Renderer &render, int autoflush = 1);
		void Flush(Renderer &render);

		void SetInput(size_t bind, TextureRef tex) { m_inputMap[bind] = tex; }
		TextureRef GetInput(size_t bind) { auto it = m_inputMap.find(bind); return it == m_inputMap.end() ? TextureRef() : it->second; }
		void ClearInput() { m_inputMap.clear(); }

		void SetOutput(TextureRef tex) { m_chainOutput = tex; }

	protected:
		void SwapBuffers();
		void FlushBuffers();
		void RenderChain(Renderer &render);

	protected:

		std::vector<Ref<EffectComposer>> m_parentSource;

		TextureRef m_texOut[4];
		Texture2D *m_pTexRead, *m_pTexWrite, *m_pTexFront, *m_pTexBack;

		Texture2DRef m_chainOutput;

		TextureSamplerRef m_textureSampler;

		MeshRef m_fullscreenquad;
		ShaderRef m_shaderFullscreenQuad;
		ShaderRef m_shaderCopyScreen;

		std::vector<EffectPassRef> m_effectChain;
		typedef std::map<size_t, TextureRef> bind_map_t;
		
		bind_map_t m_inputMap;

		struct {
			float4 s, v;
		} m_screen_params;

		bool m_singlepass;
	};

	// ---------------------------------------------------------------------------------------------------

	typedef EffectComposerRef_t EffectComposerRef;

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

		// Ezeknke itt semmi ertelme nincs meg: 
		void SetOutput(size_t bind, TextureRef tex) {m_output_map[bind] = tex; }
		void SetInput(std::string name, TextureRef tex) { m_inputMap[name] = tex; }

		void ClearOutputs() { m_output_map.clear();  }
		void ClearInputs() { m_inputMap.clear(); }

		TextureRef GetOutput(size_t bind); 
		TextureRef GetInput(std::string name);

		Ref<ShaderParameter> GetParameter() { return m_shaderParameter; }

	private:
		ShaderResRef m_shader;

		Ref<ShaderParameter> m_shaderParameter;

		typedef std::map<size_t, TextureRef> bind_map_t;
		typedef bind_map_t::iterator bind_map_it_t;
		typedef std::map<std::string, TextureRef> name_map_t;
		typedef name_map_t::iterator name_map_it_t;

		bind_map_t m_output_map;
		name_map_t m_inputMap;

	};

	// ===================================================================================================

}
