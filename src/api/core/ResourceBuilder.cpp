#include "ResourceBuilder.h"
#include "AssetFactory.h"
#include "ResourceManager.h"

using namespace Grafkit;

void Grafkit::IResourceBuilder::operator()(IResourceManager * const & assman)
{
	IResourceRef res;

	if (m_dstResource.Valid()) {
		///@todo ezzen az esettel valamit kezdeni kell majd 
		IResourceRef res = assman->Get<IResource>(m_dstResource->GetName());
	}
	else {
		IResourceRef res = assman->Get<IResource>(m_name);
	}

	if (res.Valid()) {
		m_dstResource = res;	
	}
	else {
		load(assman);

		m_dstResource->SetName(m_name);
		assman->Add(m_dstResource);
	}
}

IAssetRef IResourceBuilder::GetSourceAsset(IResourceManager * const & assman)
{
	if (assman && assman->GetAssetFactory()) 
		return assman->GetAssetFactory()->Get(m_srcName);
	else 
		return IAssetRef();
}
