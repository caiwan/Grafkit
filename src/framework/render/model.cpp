#include "stdafx.h"

#include "mesh.h"
#include "Material.h"
#include "shader.h"
#include "scene.h"

#include "Actor.h"


#include "model.h"


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

void Grafkit::Model::Render(Grafkit::Renderer & render, Scene* const & scene)
{
	if (m_material) {
		// bullshit, does not work
		//if (m_material->GetLayer() != scene->GetLayerID())
			//return;
		this->m_material->Render(render, scene->GetVShader(), scene->GetPShader());
	}

	if (m_geometryShader.Valid() && m_geometryShader->Valid())
		m_geometryShader->Get()->Bind(render);

	if (m_mesh)
		this->m_mesh->RenderMesh(render);

	//if (m_geometryShader.Valid() && m_geometryShader->Valid())
	//	m_geometryShader->Get()->Bind(render);
}

void Grafkit::Model::Build(Grafkit::Renderer & deviceContext, Scene * const & scene)
{
	if (m_mesh.Valid())
		m_mesh->Build(deviceContext, scene->GetVShader());
}

