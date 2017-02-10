/**
*/

#pragma once

#include <vector>
#include <array>

#include "../utils/memory.h"
#include "../utils/reference.h"

#include "renderer.h"
#include "dxtypes.h"

#include "mesh.h"
#include "actor.h"
#include "texture.h"
#include "Material.h"


namespace Grafkit 
{
	/**
	Stores extended data for a mesh, that makes it a model
	*/

	class Entity3D;

	__declspec(align(16)) class Model : public Grafkit::Entity3D, public AlignedNew<Model>, public Persistent
	{
	public:
		Model(MeshRef mesh = nullptr, MaterialRef material = nullptr);
		~Model();

		MeshRef GetMesh() const { return this->m_mesh; }
		void SetMesh(MeshRef model) { this->m_mesh = model; }

		MaterialRef GetMaterial() const { return this->m_material; }
		void SetMaterial(MaterialRef material) { this->m_material = material; }

		virtual void Render(Grafkit::Renderer& deviceContext, Scene* scene);
		virtual void Build(Grafkit::Renderer& deviceContext, Scene* scene);


	private:
		MaterialRef m_material;
		MeshRef m_mesh;

		PERSISTENT_DECL(Grafkit::Model, 1)
	protected:
		virtual void serialize(Archive& ar) { _serialize(ar); }
		void _serialize(Archive& ar);

	};

	typedef Ref<Model> ModelRef;
}
