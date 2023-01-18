#include "stdafx.h"

#include "mesh.h"
#include "Material.h"
#include "shader.h"

#include"Texture.h" //?

#include "SceneGraph.h"

#include "model.h"


PERSISTENT_IMPL(Grafkit::Model);

using namespace Grafkit;

// ==================================================================

Model::Model() {
}

Model::Model(MeshRef mesh) :
    m_mesh(mesh) {
}

Model::Model(MeshRef mesh, MaterialRef material) :
    m_material(material)
    , m_mesh(mesh) {
}

Model::~Model() {
}

MeshRef Model::GetMesh() const { return this->m_mesh; }

void Model::SetMesh(MeshRef model) { this->m_mesh = model; }

MaterialRef Model::GetMaterial() const { return this->m_material; }

void Model::SetMaterial(MaterialRef material) { this->m_material = material; }

// ==================================================================

void Model::Calculate(Renderer& deviceContext, ActorRef parent) {
}

void Model::Render(Renderer& render, SceneGraph* const & scene)
{
    if (m_material) { this->m_material->Render(render, scene->GetVShader(), scene->GetPShader()); }

    if (m_geometryShader.Valid() && m_geometryShader->Valid())
        m_geometryShader->Get()->Bind(render);

    if (m_mesh)
        this->m_mesh->RenderMesh(render);
}

void Model::Build(Renderer& deviceContext, SceneGraph* const & scene)
{
    if (m_mesh.Valid())
        m_mesh->Build(deviceContext, scene->GetVShader());
}

void Model::SetGeometryShader(ShaderResRef shader) { m_geometryShader = shader; }

ShaderResRef Model::GetGeometryShader() const { return m_geometryShader; }

void Model::Serialize(Archive& ar)
{
    Object::_Serialize(ar);
    // Materials will be persisted ina separate map
    //PERSIST_REFOBJECT(ar, m_material);
    PERSIST_REFOBJECT(ar, m_mesh);
}
