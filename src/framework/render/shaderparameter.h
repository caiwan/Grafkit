#pragma once 

#include "renderer.h"
#include "shader.h"

#include "texture.h"

#include "renderelement.h"

namespace Grafkit {

	class ShaderParameter;

	class ShaderParameter
#if 0
		: public Grafkit::IRenderElement
#endif
	{
	public:
		ShaderParameter();
		virtual ~ShaderParameter();

		void Initialize(Renderer &render, ShaderResRef shader);

		ShaderRef GetShader() { return m_targetShader->Get(); }

		// 
		void SetParam(std::string name, void *ptr);
		void SetTexture(std::string name, Texture1DResRef texture) { SetATexture(name, texture); }
		void SetTexture(std::string name, Texture2DResRef texture) { SetATexture(name, texture); }
		//void SetTexture(std::string name, Texture3DResRef texture){ SetATexture(name, texture); }
		void SetTexture(std::string name, TextureCubeResRef texture) { SetATexture(name, texture); }
		void SetSampler(std::string name, TextureSamplerRef sampler);

		//
		void BindParameters(Renderer &render);

	protected:
		void SetATexture(std::string name, Ref<Resource<ATexture>> texture);
		void UpdateTargets();

	private:
		ShaderResRef m_targetShader;
		ShaderRef m_lastShader;

		struct Param_T {
			int32_t id;
			void * p;
		};

		struct Texture_T {
			int32_t id;
			Ref<Resource<ATexture>> texture;
		};

		struct Sampler_T {
			int32_t id;
			TextureSamplerRef sampler;
		};

		std::map<std::string, Param_T> m_paramMap;
		std::map<std::string, Texture_T> m_textureMap;
		std::map<std::string, Sampler_T> m_smaplerMap;

		bool m_refresh;
	};

}
