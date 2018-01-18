#pragma once

#include "stdafx.h"
#include <string>
#include <map>

#include "renderer.h"
#include "renderparameter.h"

namespace Grafkit {

	class RenderParameter;
	class IRenderParameterTarget;

	/*
	Render element is a a container for parameter templates
	*/
	class IRenderElement : public virtual Referencable
	{
	public:
		IRenderElement(){}
		virtual ~IRenderElement(){}

	public:
		std::string GetName() { return this->m_name; }
		void SetName(std::string name) { m_name = name; }

		void BindParameters(Renderer &render);
		
		void SetParameter(Ref<RenderParameter> parameter);
		void ClearParameter(Ref<RenderParameter> parameter);

		Ref<RenderParameter> FindParameter(std::string name);

	protected:
		void AddTarget(Ref<IRenderParameterTarget> target);

		virtual void OnBeforeBind(Grafkit::Renderer &render){}
		virtual void OnAfterBind(Grafkit::Renderer &render){}

	protected:
		std::string m_name;

		std::map<std::string, Ref<RenderParameter>> m_parameterMap;
		std::map<std::string, Ref<IRenderParameterTarget>> m_targetMap;
	};

	// An abstract writer for each render type of render element targets
	class IRenderParameterTarget : public virtual Referencable {
		friend class IRenderElement;
	public:
		IRenderParameterTarget(std::string name) : m_name(name){}
		virtual ~IRenderParameterTarget(){}

		std::string GetName() { return this->m_name; }
		void SetName(std::string name) { m_name = name; }

	protected:
		virtual void WriteTarget(Renderer &render, IRenderElement * const & targetElement, const RenderParameter * const & sourceParameter) = 0;

		std::string m_name;
	};

}