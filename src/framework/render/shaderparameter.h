#pragma once 

#include "renderer.h"
#include "shader.h"

#include "renderparameter.h"

namespace Gafkit {

	class ShaderParameter;

	class ShaderParameter : public Grafkit::IRenderElement {
	public:
		ShaderParameter();
		virtual ~ShaderParameter();

		void Initialize(Renderer &render, ShaderResRef shader);

		ShaderRef GetShader() { return m_targetShader.Get(); }

	protected:
		void OnBeforeBind(Renderer &render);
		void AddTargets();
		void UpdateTargets();

	private:
		ShaderResRef m_targetShader;
		ShaderRef m_lastShader;

	};



}
