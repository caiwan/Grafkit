#include "model.h"
#include "texture.h"

using namespace FWrender;

// ==================================================================

Model::Model() : Mesh()
{
}


Model::~Model()
{
}

// ==================================================================

void FWrender::Model::Render(FWrender::Renderer& deviceContext)
{
	if (m_material) this->m_material->Render(deviceContext);
	this->RenderMesh(deviceContext);
}