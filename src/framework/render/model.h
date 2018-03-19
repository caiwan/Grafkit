/**
*/

#pragma once

#include <vector>
#include <array>

#include "common.h"

#include "render/Actor.h"
#include "render/renderer.h"

#include "mesh.h"
#include "Material.h"

namespace Grafkit 
{
	/**
	Stores extended data for a mesh, that makes it a model
	*/

	__declspec(align(16)) class Model : public Grafkit::Entity3D, public AlignedNew<Model>
	{
	public:
		Model(MeshRef mesh = nullptr, MaterialRef material = nullptr);
		~Model();

		MeshRef GetMesh() const     { return this->m_mesh; }
		void SetMesh(MeshRef model) { this->m_mesh = model; }

		MaterialRef GetMaterial() const		{ return this->m_material; }
		void SetMaterial(MaterialRef material) { this->m_material = material; }

		virtual void Render(Grafkit::Renderer& deviceContext, SceneGraph* const & scene);
		virtual void Build(Grafkit::Renderer& deviceContext, SceneGraph* const & scene);

		void SetGeometryShader(ShaderResRef shader) { m_geometryShader = shader; }
		ShaderResRef GetGeometryShader() { return m_geometryShader; }

	private:
		MaterialRef m_material;
		MeshRef m_mesh;

		ShaderResRef m_geometryShader;

		PERSISTENT_DECL(Grafkit::Model, 1)
	protected:
		virtual void serialize(Archive& ar) { _serialize(ar); }
		void _serialize(Archive& ar);

	};

}
