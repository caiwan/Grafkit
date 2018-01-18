#include "renderelement.h"
#include "renderparameter.h"

using namespace Grafkit;

void Grafkit::IRenderElement::BindParameters(Renderer & render)
{
	OnBeforeBind(render);
	for (auto it = m_parameterMap.begin(); it != m_parameterMap.end(); it++) {
		auto parameter = it->second.Get();
		auto target = parameter->m_target;
		target->WriteTarget(render, this, parameter);
	}
	OnAfterBind(render);
}

void Grafkit::IRenderElement::SetParameter(Ref<RenderParameter> parameter)
{
	// skip check if parameter refers to a target
	m_parameterMap[parameter->m_name] = parameter;
}

void Grafkit::IRenderElement::ClearParameter(Ref<RenderParameter> parameter)
{
	auto it = m_parameterMap.find(parameter->m_name);
	if (it != m_parameterMap.end())
		m_parameterMap.erase(it);
}

Ref<RenderParameter> Grafkit::IRenderElement::FindParameter(std::string name)
{
	auto it = m_parameterMap.find(name);

	if (it != m_parameterMap.end())
		return new RenderParameter(it->second);
	return nullptr;
}

void Grafkit::IRenderElement::AddTarget(Ref<IRenderParameterTarget> target)
{
	m_targetMap[target->m_name] = target;
}
