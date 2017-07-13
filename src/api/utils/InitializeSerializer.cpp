#include "stdafx.h"

#include "persistence/dynamics.h"
#include "persistence/persistence.h"
#include "InitializeSerializer.h"

#include "render/predefs.h"

#include "render/mesh.h"
#include "render/model.h"

#include "render/animation.h"

Grafkit::ClonableInitializer::ClonableInitializer()
{
	Grafkit::AddClonable mesh("Grafkit::Mesh", new Grafkit::Mesh::Factory());
	Grafkit::AddClonable model("Grafkit::Model", new Grafkit::Model::Factory());
	Grafkit::AddClonable actorAnimation("Grafkit::ActorAnimation", new Grafkit::ActorAnimation::Factory());
}
