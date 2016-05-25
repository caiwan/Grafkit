#include "../stdafx.h"

#include "model.h"
#include "Scene.h"

using namespace Grafkit;

// ==================================================================

Model::Model() : Mesh()
{
}


Model::~Model()
{
}

// ==================================================================

void Grafkit::Model::Render(Grafkit::Renderer & render, Scene* scene)
{
	if (m_material) 
		this->m_material->Render(render, scene->GetFShader());

	this->RenderMesh(render);
}
