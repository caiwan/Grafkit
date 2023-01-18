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

	scene->GetFShader()->Render(render);
	scene->GetVShader()->Render(render);

	this->RenderMesh(render);
}