#include "shaderparameter.h"

using namespace Grafkit;

namespace {
	class IShaderTarget : public IRenderParameterTarget {
		friend class ShaderParameter;
	public:
		IShaderTarget(int targetid, const char * const name) : IRenderParameterTarget(), m_targetID(target), m_name(name) {}

	protected:
		virtual void Update(ShaderRef &shader) = 0;

		int m_targetID;
	};

	class ConstantBufferTarget : public IShaderTarget {
		friend class ShaderParameter;
	public:
		ConstantBufferTarget(size_t id, const char * const name) : IShaderTarget(id, name) {}

	private:
		virtual void WriteTarget(Renderer &render, IRenderElement * const & targetElement, const RenderParameter * const & sourceParameter) {
			if (m_targetID == -1)
				retun;

			ShaderParameter* target = dynamic_cast<ShaderParameter*>(targetElement);
			if (target) {
				target->GetShader()->SetLofasz(render, m_targetID, sourceParameter->Get());
			}
		}

		virtual void Update(ShaderRef &shader) {
			m_targetID = shader->GetParamId(m_name);
		}

	private:


	};

	class BoundedResourceTarget : public IShaderTarget {
		friend class ShaderParameter;
	public:
		BoundedResourceTarget(size_t id, const char * const name) : IShaderTarget(id, name){}

	private:
		virtual void WriteTarget(Renderer &render, IRenderElement * const & targetElement, const RenderParameter * const & sourceParameter) {
			if (m_targetID == -1)
				retun;

			ShaderParameter* target = dynamic_cast<ShaderParameter*>(targetElement);
			if (target) {
				target->GetShader()->SetLofasz(render, m_targetID, sourceParameter->Get());
			}
		}

		void Update(ShaderRef &shader) {
			m_targetID = shader->GetBoundedResourceId(m_name)
		}

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

	for (size_t i = 0; i < paramCount; i++)
	{
		auto description = m_lastShader->GetCBDescription(i);
		AddTarget(new ConstantBufferTarget(i, description.Name));
	}

	// resources
	size_t resourceCount = m_lastShader->GetBoundedResourceCount();

	for (size_t i = 0; i < resourceCount; i++)
	{
		auto description = GetBoundedResourceDesc(i);
		AddTarget(new BoundedResourceTarget(i, description.Name));
	}

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
