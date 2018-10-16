#include "stdafx.h"

#include "InitializeSerializer.h"

#include "utils/persistence/dynamics.h"

#include "render/mesh.h"
#include "render/model.h"
#include "render/material.h"
#include "render/texture.h" // ? 

#include "render/Light.h"
#include "render/camera.h"

//#include "render/animation.h"

#define ADD_CLONABLE(_name, _clazzname)\
	Grafkit::AddClonable _name(#_clazzname, new _clazzname::Factory())

Grafkit::ClonableInitializer::ClonableInitializer()
{
    // Hello ?

	//ADD_CLONABLE(mesh, Grafkit::Mesh);
	//ADD_CLONABLE(model, Grafkit::Model);
	//ADD_CLONABLE(material, Grafkit::Material);
	//ADD_CLONABLE(camera, Grafkit::Camera);
	//ADD_CLONABLE(light, Grafkit::Light);
	//ADD_CLONABLE(actorAnimation, Grafkit::ActorAnimation);
}
