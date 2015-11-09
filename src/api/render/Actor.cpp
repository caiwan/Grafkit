
Model::Model() : Mesh()
{
}


Model::ActorModel(const Model& other) : Mesh(other)
{
	// TBD
}


Model::~Model()
{
	this->Shutdown();
}

// ==================================================================

ID3D11ShaderResourceView* Model::GetTexture()
{
	return m_texture ? m_texture->GetTexture() : 0;
}


void Model::Shutdown()
{
	ShutdownBuffers();
	return;
}

void Model::Render(ID3D11DeviceContext* deviceContext)
{
	RenderBuffers(deviceContext);
	return;
}