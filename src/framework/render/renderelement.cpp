#include "renderelement.h"
#include "renderparameter.h"

using namespace Grafkit;

void Grafkit::IRenderElement::BindParameters(Renderer & render)
{
	OnBeforeBind(render);
	for (auto it = m_parameterMap.begin(); it != m_parameterMap.end(); it++) {
		for (auto sit = it->second.begin(); sit != it->second.end(); sit++) {
			auto parameter = sit->Get();
			auto target = parameter->m_target;
			target->WriteTarget(render, this, parameter);
		}
	}
	OnAfterBind(render);
}

void Grafkit::IRenderElement::SetParameter(Ref<RenderParameter> parameter)
{
	// skip check if parameter refers to a target
	m_parameterMap[parameter->m_name];
	m_parameterMap[parameter->m_name].push_back(parameter);
}

void Grafkit::IRenderElement::ClearParameter(Ref<RenderParameter> parameter)
{
	auto it = m_parameterMap.find(parameter->m_name);
	if (it != m_parameterMap.end())
		m_parameterMap.erase(it);
}

void Grafkit::IRenderElement::AddTarget(Ref<IRenderParameterTarget> target)
{
	m_targetMap[target->m_name];
	m_targetMap[target->m_name].push_back(target);
}
