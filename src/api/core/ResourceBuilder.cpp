#include "ResourceBuilder.h"
#include "AssetFactory.h"
#include "ResourceManager.h"

using namespace Grafkit;

void Grafkit::IResourceBuilder::operator()(IResourceManager * const & assman)
{
	if (m_dstResource.Valid()) {
		IResourceRef res = assman->Get<IResource>(m_dstResource->GetName());
		if (res.Valid()) {
			m_dstResource = res;
			return; 
		}
	}

	load(assman);
	
	m_dstResource->SetName(m_srcName);
	assman->Add(m_dstResource);
	
}

IAssetRef IResourceBuilder::GetSourceAsset(IResourceManager * const & assman)
{
	if (assman && assman->GetAssetFactory()) 
		return assman->GetAssetFactory()->Get(m_srcName);
	else 
		return IAssetRef();
}
