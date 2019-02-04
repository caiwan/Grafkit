#include "AnimationLoader.h"

#include "animation/actoranimation.h"

#include "core/resource.h"
#include "resource/ResourceManager.h"
#include "utils/asset/AssetFactory.h"

#include "core/exceptions.h"
#include "utils/persistence/archive.h"
#include "utils/persistence/persistence.h"
#include "core/asset.h"


using namespace Grafkit;
using namespace FWdebugExceptions;

#if 0
void ActorAnimationLoader::Load(IResourceManager* const& resman, IResource* source)
{
    ActorAnimationResRef dstAnimation = dynamic_cast<ActorAnimationRes*>(source);
    assert(dstAnimation);

    ActorAnimationRef animation;

    if (m_params.sourceName.empty())
    {
        animation = new ActorAnimation();
        animation->Initialize();

        animation->SetName(m_name);
        animation->SetUuid(m_uuid);
    }
    else
    {
        try
        {
            StreamRef resource = resman->GetAssetFactory()->Get(m_params.sourceName);
            //ArchiveAsset archiveAsset(resource);
            //animation = dynamic_cast<ActorAnimation*>(ActorAnimation::Load(archiveAsset));
        }
        catch (std::exception& e)
        {
            LOGGER(Log::Logger().Warn("Cannot load cache %s", m_params.sourceName.c_str()));
            animation = new ActorAnimation();
            animation->Initialize();

            animation->SetName(m_name);
            animation->SetUuid(m_uuid);
        }
    }

    if (!animation)
        THROW_EX_DETAILS(NullPointerException, "Could not load Animation");

    dstAnimation->AssingnRef(animation);
}


void ActorAnimationLoader::Initialize(Renderer& render, IResourceManager* const& resman, IResource* source) {
    // Assign target
}
#endif

//void ActorAnimationLoader::Serialize(Archive& ar) {
//}
void ActorAnimationLoader::Load(const std::shared_ptr<IResource>& resource, ResourceManager& resourceManager) {
}

void ActorAnimationLoader::Initialize(const std::shared_ptr<IResource>& resource, ResourceManager& resourceManager) {
}
