/**
*/

#ifndef _Model_H_
#define _Model_H_

#include "renderer.h"
#include "dxtypes.h"

#include "../core/reference.h"

#include "mesh.h"
#include "actor.h"
#include "texture.h"

#include <vector>
#include <array>

namespace FWrender 
{
	/**
	Stores extended data for a mesh, that makes it a model
	*/

	class Entity3D;

	class Model : public virtual Referencable, 
		public FWrender::Mesh, public FWrender::Entity3D
	{
	public:
		Model();
		Model(const Model& other);

		~Model();

		//void addMesh(MeshRef mesh, /*materials, textures*/);

		inline void setTexture(TextureRef texture, int n = 0);
		inline TextureRef& getTexture(int n = 0);

	private:
		// + material
		TextureRef m_textures[MULTITEXTURE_MAX];

	};

	typedef Ref<Model> ModelRef;
}

#endif
