#pragma once 

#include "renderer.h"
#include "shader.h"
#include "texture.h"

namespace Grafkit {

	class ShaderParameter;

	class ShaderParameter : virtual public Referencable
	{
	public:
		ShaderParameter();
		virtual ~ShaderParameter();

		void Initialize(Renderer &m_render, ShaderResRef shader);

		ShaderRes GetShader() const { return m_targetShader; }

		// 
		void SetParam(std::string name, void *ptr);
		void SetTexture(std::string name, const Texture1DRes &texture) { SetATexture(name, texture.CastTo<ATexture>()); }
		void SetTexture(std::string name, const Texture2DRes &texture) { SetATexture(name, texture.CastTo<ATexture>()); }
		//void SetTexture(std::string name, Texture3DRes texture){ SetATexture(name, texture.CastTo<ATexture>()); }
		void SetTexture(std::string name, const TextureCubeRes &texture) { SetATexture(name, texture.CastTo<ATexture>()); }
		void SetSampler(std::string name, const TextureSamplerRes &sampler);

		//
		void BindParameters(Renderer &m_render);

	protected:
        void SetATexture(std::string name, Resource<ATexture> texture);
		void UpdateTargets();

	private:
		ShaderRes m_targetShader;
		ShaderRef m_lastShader;

		struct Param_T {
			int32_t id;
			void * p;
		};

		struct Texture_T {
			int32_t id;
			Resource<ATexture> texture;
		};

		struct Sampler_T {
			int32_t id;
			TextureSamplerRes sampler;
		};

		std::map<std::string, Param_T> m_paramMap;
		std::map<std::string, Texture_T> m_textureMap;
		std::map<std::string, Sampler_T> m_smaplerMap;

		bool m_refresh;
	};

}
