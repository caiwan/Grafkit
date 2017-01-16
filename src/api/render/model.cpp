#include "stdafx.h"

#include "model.h"
#include "Scene.h"

using namespace Grafkit;

// ==================================================================
PERSISTENT_IMPL(Grafkit::Model);

void Grafkit::Model::_serialize(Archive & ar)
{
	this->Entity3D::_serialize(ar);
	PERSIST_REFOBJECT(ar, m_mesh);
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
		this->m_material->Render(render, scene->GetPShader());

	if (m_mesh)
		this->m_mesh->RenderMesh(render);
}

void Grafkit::Model::Build(Grafkit::Renderer & deviceContext, Scene * scene)
{
	if (m_mesh.Valid())
		m_mesh->Build(deviceContext, scene->GetVShader());
}
