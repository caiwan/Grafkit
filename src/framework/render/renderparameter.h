#pragma once

#include "stdafx.h"

#include <string>



namespace Grafkit{

	class IRenderElement;
	class IRenderParameterTarget;

	/* Holds data for a single parameter of a rendering element (usually a struct)
	*/
	class RenderParameter
	{
		friend class IRenderParameterTarget;
		friend class IRenderElement;
	public:

		RenderParameter(Ref<IRenderParameterTarget> target){}
		~RenderParameter(){}

		template<typename T> T* Get() const { return *((T*)m_data); }
		template<typename T> void Set(T *&t) { (T*)m_data = t; }

		std::string GetName() { return this->m_name; }
		void SetName(std::string name) { m_name = name; }

	protected:
		std::string m_name;
		void *m_data;
		size_t m_size;
		Ref<IRenderParameterTarget> m_target;
	};

}
