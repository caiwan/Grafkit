#include "stdafx.h"

#include "ResourceBuilder.h"
#include "AssetFactory.h"
#include "ResourceManager.h"

using namespace Grafkit;

IAssetRef IResourceBuilder::GetSourceAsset(IResourceManager * const & assman) const
{
	if (assman && assman->GetAssetFactory()) 
		return assman->GetAssetFactory()->Get(m_sourceName);
    return IAssetRef();
}
