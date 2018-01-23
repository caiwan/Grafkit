#pragma once

#include "stdafx.h"

#include <string>

namespace Grafkit{

	class IRenderElement;
	class IRenderParameterTarget;

	/* Holds data for a single parameter of a rendering element (usually a struct)
	*/
	class RenderParameter : virtual public Referencable
	{
		friend class IRenderParameterTarget;
		friend class IRenderElement;
	public:

		RenderParameter(Ref<IRenderParameterTarget> target){}
		~RenderParameter(){}

		const void* Get() const { return m_data; }
		void Set(void* p) { m_data = p; }

		std::string GetName() { return this->m_name; }
		void SetName(std::string name) { m_name = name; }

	protected:
		std::string m_name;
		void *m_data;
		size_t m_size;
		Ref<IRenderParameterTarget> m_target;
	};

}
