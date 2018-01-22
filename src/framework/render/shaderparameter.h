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



	/// ========================================================================================================================================================

	class IShaderTarget : public IRenderParameterTarget {
		friend class ShaderParameter;
	public:
		IShaderTarget(int targetID, const char * const name) : IRenderParameterTarget(name), m_targetID(targetID) {}

	protected:
		virtual void Update(ShaderRef &shader) = 0;
		int m_targetID;
	};

	/// ========================================================================================================================================================

	class ConstantBufferTarget : public IShaderTarget {
		friend class ShaderParameter;
	public:
		ConstantBufferTarget(size_t id, const char * const name) : IShaderTarget(id, name) {}

	private:
		virtual void WriteTarget(Renderer &render, IRenderElement * const & targetElement, const RenderParameter * const & sourceParameter);
		virtual void Update(ShaderRef &shader);
	};

	/// ========================================================================================================================================================

	class BoundedResourceTarget : public IShaderTarget {
		friend class ShaderParameter;
	public:
		BoundedResourceTarget(size_t id, const char * const name) : IShaderTarget(id, name) {}

	private:
		virtual void WriteTarget(Renderer &render, IRenderElement * const & targetElement, const RenderParameter * const & sourceParameter);
		void Update(ShaderRef &shader);

	};

}
