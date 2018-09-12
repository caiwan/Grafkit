#include "stdafx.h"

#include "ResourceBuilder.h"
#include "ResourceManager.h"

#include "core/Asset.h"
#include "utils/asset/AssetFactory.h"

using namespace Grafkit;

IAssetRef IResourceBuilder::GetSourceAsset(IResourceManager* const & assman) const
{
    if (assman && assman->GetAssetFactory())
        return assman->GetAssetFactory()->Get(m_sourceName);
    return IAssetRef();
}
