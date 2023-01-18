/**
*/

#ifndef _Model_H_
#define _Model_H_

#include "renderer.h"
#include "dxtypes.h"

#include "reference.h"

#include "mesh.h"
#include "actor.h"
#include "texture.h"
#include "Material.h"

#include <vector>
#include <array>

namespace FWrender 
{
	/**
	Stores extended data for a mesh, that makes it a model
	*/

#define MODEL_BUCKET "model:"

	class Entity3D;

	class Model : /*public virtual Referencable,*/ public FWrender::Mesh, public FWrender::Entity3D
	{
		public:
			Model();
			~Model();

			virtual void Render(FWrender::Renderer& deviceContext);

			virtual const char* GetBucketID() { return MODEL_BUCKET; }
	};

	typedef Ref<Model> ModelRef;
}

#endif //_Model_H_
