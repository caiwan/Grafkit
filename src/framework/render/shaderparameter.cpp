#include "shaderparameter.h"

#include "renderelement.h"
#include "renderparameter.h"

using namespace Grafkit;

namespace {
	class IShaderTarget : public IRenderParameterTarget {
		friend class ShaderParameter;
	public:
		IShaderTarget(int targetID, const char * const name) : IRenderParameterTarget(name), m_targetID(targetID){}

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
				return;

			ShaderParameter* target = dynamic_cast<ShaderParameter*>(const_cast<IRenderElement*>(targetElement));
			if (target) {
				target->GetShader()->SetParam(render, m_targetID, sourceParameter->Get());
			}
		}

		virtual void Update(ShaderRef &shader) {
			m_targetID = shader->GetParamId(m_name);
		}

	};

	class BoundedResourceTarget : public IShaderTarget {
		friend class ShaderParameter;
	public:
		BoundedResourceTarget(size_t id, const char * const name) : IShaderTarget(id, name){}

	private:
		virtual void WriteTarget(Renderer &render, IRenderElement * const & targetElement, const RenderParameter * const & sourceParameter) {
			if (m_targetID == -1)
				return;

			ShaderParameter* target = dynamic_cast<ShaderParameter*>(targetElement);
			if (target) {
				target->GetShader()->SetBoundedResourcePointer(render, m_targetID, sourceParameter->Get());
			}
		}

		void Update(ShaderRef &shader) {
			m_targetID = shader->GetBoundedResourceId(m_name);
		}

	};
}

Grafkit::ShaderParameter::ShaderParameter() : IRenderElement()
{
}

Grafkit::ShaderParameter::~ShaderParameter()
{
}

void Grafkit::ShaderParameter::Initialize(Renderer & render, ShaderResRef shader)
{
	m_targetShader = shader;
	m_lastShader = m_targetShader->Get();

	// add targets

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
		auto description = m_lastShader->GetBoundedResourceDesc(i);
		AddTarget(new BoundedResourceTarget(i, description.Name));
	}
}

void Grafkit::ShaderParameter::OnBeforeBind(Renderer & render)
{
	if (m_lastShader.Get() != m_targetShader->Get()) {
		void UpdateTargets();
	}
}


void Grafkit::ShaderParameter::UpdateTargets()
{
	m_lastShader = m_targetShader->Get();
	for (auto it = m_targetMap.begin(); it != m_targetMap.end(); it++) {
		IShaderTarget* target = dynamic_cast<IShaderTarget*>(it->second.Get());
		if (target)
			target->Update(m_lastShader);
	}
}
