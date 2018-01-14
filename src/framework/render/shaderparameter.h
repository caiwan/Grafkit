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

	protected:
		void OnBeforeBind(Renderer &render);
		void AddTargets();
		void UpdateTargets();

	private:
		ShaderResRef m_targetShader;
		ShaderRef m_lastShader;

	};



}
