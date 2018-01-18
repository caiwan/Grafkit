#pragma once 

#include "renderer.h"
#include "shader.h"

#include "renderelement.h"

namespace Grafkit {

	class ShaderParameter;

	class ShaderParameter : public Grafkit::IRenderElement {
	public:
		ShaderParameter();
		virtual ~ShaderParameter();

		void Initialize(Renderer &render, ShaderResRef shader);

		ShaderRef GetShader() { return m_targetShader->Get(); }

	protected:
		void OnBeforeBind(Renderer &render);
		void UpdateTargets();

	private:
		ShaderResRef m_targetShader;
		ShaderRef m_lastShader;

	};

}
