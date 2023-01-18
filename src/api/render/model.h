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

	class Model : public virtual Referencable, public Actor, public Mesh{
	public:
		Model();
		~Model();

		void addMesh(MeshRef mesh, /*materials, textures*/);

	private:
		/** @todo 
		Ezt kesobb valaszd szet. Egy model, csak egy mesht, es annak egy materialjat tarolja. 
		Legyen egy absztrakt osztaly - 3Dentity - az Actor helyett, amibol szarmazik, es az
		Actor ezeket gyujtse
		*/

		struct entity_t{
			MeshRef mesh;
			// + material
			// + textures
		};
		std::vector<struct entity_t> entities;
	};
}

#endif
