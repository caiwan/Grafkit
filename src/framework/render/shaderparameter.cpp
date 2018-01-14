#include "shaderparameter.h"

using namespace Grafkit;

namespace {
	class IShaderTarget : public IRenderParameterTarget {
		friend class ShaderParameter;
	public:
		IShaderTarget() : IRenderParameterTarget(), m_targetID(-1) {}
	protected:
		virtual void Update(ShaderRef &shader) = 0;

		int m_targetID;
	};

	class ConstantBufferTarget : public IShaderTarget {
		friend class ShaderParameter;
	public:
		ConstantBufferTarget() : IShaderTarget() {}
	private:
		virtual void WriteTarget(Renderer &render, IRenderElement * const & targetElement, const RenderParameter * const & sourceParameter) = 0;
		virtual void Update(ShaderRef &shader) = 0;

	};

	class BoundedResourceTarget : public IShaderTarget {
		friend class ShaderParameter;
	public:
		BoundedResourceTarget() : IShaderTarget();
	private:
		virtual void WriteTarget(Renderer &render, IRenderElement * const & targetElement, const RenderParameter * const & sourceParameter) = 0;
		void Update(ShaderRef &shader);

	};
}

Gafkit::ShaderParameter::ShaderParameter() : IRenderElement()
{
}

Gafkit::ShaderParameter::~ShaderParameter()
{
}

void Gafkit::ShaderParameter::Initialize(Renderer & render, ShaderResRef shader)
{
	m_targetShader = shader;
	m_lastShader = m_targetShader->Get();
	AddTargets();
}

void Gafkit::ShaderParameter::OnBeforeBind(Renderer & render)
{
	if (m_lastShader.Get() != m_targetShader->Get()) {
		void UpdateTargets();
	}
}

void Gafkit::ShaderParameter::AddTargets()
{
	m_lastShader = m_targetShader->Get();

	// cbuffers
	size_t paramCount = m_lastShader->GetParamCount();
	size_t paramId = 0;
	D3D11_SHADER_BUFFER_DESC paramDesc = m_lastShader->GetCBDescription(paramId);

	// ...

	// resources
	size_t resourceCount = m_lastShader->GetBoundedResourceCount();
	size_t resourceId = 0;
	D3D11_SHADER_INPUT_BIND_DESC resourceDesc = GetBoundedResourceDesc(resourceId);

	// ...
}

void Gafkit::ShaderParameter::UpdateTargets()
{
	m_lastShader = m_targetShader->Get();
	for (auto it = m_targetMap.begin(); it != m_targetMap.end(); i++) {
		IShaderTarget* target = dynamic_cast<IShaderTarget*>(it->second);
		if (target)
			target->Update(m_lastShader);
	}
}
