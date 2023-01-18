/**
*/

#ifndef _Model_H_
#define _Model_H_

#include "renderer.h"
#include "dxtypes.h"

#include "../core/reference.h"
#include "mesh.h"
#include "Actor.h"

#include <vector>
#include <array>

namespace FWrender 
{
	/**
	Stores extended data for a mesh, that makes it a model
	*/

	class Model : public virtual Referencable, public Entity3D, public Mesh{
	public:
		Model();
		~Model();

		//void addMesh(MeshRef mesh, /*materials, textures*/);

	private:
		//MeshRef mesh;

		// + material
		// + texture 

	};
}

#endif
