#include "DemoAnimationLoader.h"
#include "demo.h"

#include "utils/resource.h"
#include "utils/ResourceManager.h"
#include "utils/AssetFactory.h"

#include "utils/exceptions.h"
#include "utils/persistence/archive.h"
#include "utils/persistence/persistence.h"
#include "utils/asset.h"

using namespace FWdebugExceptions;

namespace Grafkit {
    class ArchiveAsset;
}

using namespace GkDemo;
using namespace Grafkit;


DemoAnimationLoader::DemoAnimationLoader(std::string name, std::string sourcename, std::string uuid, std::string targetUuid) : IAnimationLoader(name, sourcename, uuid, targetUuid){
}

IResource* DemoAnimationLoader::NewResource() {
    return new Resource<DemoAnimation>();
}

void DemoAnimationLoader::Load(IResourceManager* const& resman, IResource* source) {
    Ref<Resource<DemoAnimation>> dstAnimation = dynamic_cast<Resource<DemoAnimation>*>(source);
    assert(dstAnimation);

    Ref<DemoAnimation> animation;

    if (m_sourceName.empty())
    {
        animation = new DemoAnimation();
        animation->Initialize();

        animation->SetName(m_name);
        animation->SetUuid(m_uuid);
    }
    else
    {
        try
        {
            IAssetRef resource = GetSourceAsset(resman);
            ArchiveAsset archiveAsset(resource);
            animation = dynamic_cast<DemoAnimation*>(DemoAnimation::Load(archiveAsset));
        }
        catch (std::exception& e)
        {
            LOGGER(Log::Logger().Warn("Cannot load cache %s", m_sourceName.c_str()));
            animation = new DemoAnimation();
            animation->Initialize();

            animation->SetName(m_name);
            animation->SetUuid(m_uuid);
        }
    }


    if (!animation)
        THROW_EX_DETAILS(NullPointerException, "Could not load Animation");
    LOGGER(Log::Logger().Debug("Animation loaded %d %s Ptr: %#x", m_name.c_str(), m_uuid.c_str(), animation.Get()));

    dstAnimation->AssingnRef(animation);
}
