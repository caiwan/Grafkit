#include "stdafx.h"

#include "model.h"
#include "Scene.h"

using namespace Grafkit;

// ==================================================================
PERSISTENT_IMPL(Grafkit::Model);

void Grafkit::Model::_serialize(Archive & ar)
{
	this->Entity3D::_serialize(ar);
	// ar & PERSIST_OBJECT(m_material);
	ar & PERSIST_OBJECT(m_mesh);
}

Grafkit::Model::Model(MeshRef mesh, MaterialRef material) :
	m_mesh(mesh),	
	m_material(material)
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

	if (m_mesh)
		this->m_mesh->RenderMesh(render);
}