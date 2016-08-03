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

#define MODEL_BUCKET "model"

	class Entity3D;

	__declspec(align(16)) class Model : public Grafkit::Mesh, public Grafkit::Entity3D, public AlignedNew<Model>
	// class Model : public Grafkit::Mesh, public Grafkit::Entity3D
	{
	public:
		Model();
		~Model();

		MaterialRef &GetMaterial() { return this->m_material; }
		void SetMaterial(MaterialRef material) { this->m_material = material; }

		virtual void Render(Grafkit::Renderer& deviceContext, Scene* scene);

		virtual const char* GetBucketID() { return MODEL_BUCKET; }

	private:
		MaterialRef m_material;
	};

	typedef Ref<Model> ModelRef;
}
