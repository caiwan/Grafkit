#include "AnimationLoader.h"

#include "animation/actoranimation.h"

#include "utils/resource.h"
#include "utils/ResourceManager.h"
#include "utils/AssetFactory.h"

#include "utils/exceptions.h"
#include "utils/persistence/archive.h"
#include "utils/persistence/persistence.h"
#include "utils/asset.h"

using namespace Grafkit;
using namespace FWdebugExceptions;

ActorAnimationLoader::ActorAnimationLoader(std::string name, std::string sourcename, std::string uuid, std::string targetUuid): IAnimationLoader(name, sourcename, uuid, targetUuid) {
}

IResource* ActorAnimationLoader::NewResource() { return new ActorAnimationRes(); }

void ActorAnimationLoader::Load(IResourceManager* const& resman, IResource* source)
{
    ActorAnimationResRef dstAnimation = dynamic_cast<ActorAnimationRes*>(source);
    assert(dstAnimation);

    ActorAnimationRef animation;

    if (m_sourceName.empty())
    {
        animation = new ActorAnimation();
        animation->Initialize();
    }
    else
    {
        try
        {
            IAssetRef resource = GetSourceAsset(resman);
            ArchiveAsset archiveAsset(resource);
            animation = dynamic_cast<ActorAnimation*>(ActorAnimation::Load(archiveAsset));
        }
        catch (std::exception& e)
        {
            LOGGER(Log::Logger().Warn("Cannot load cache %s", m_sourceName.c_str()));
            animation = new ActorAnimation();
            animation->Initialize();
        }
    }

    //if (!m_target.empty())
    //{
    //    Ref<Resource<Actor>> actor = resman->GetByUuid<Resource<Actor>>(m_target);
    //    if (actor)
    //    {
    //        animation->SetActor(static_cast<Ref<Actor>>(*actor));
    //    }
    //}

    if (!animation)
    THROW_EX_DETAILS(NullPointerException, "Could not load Animation");

    animation->SetName(m_name);
    animation->SetUuid(m_uuid);

    LOGGER(Log::Logger().Debug("Animation loaded %d %s Ptr: %#x", m_name.c_str(), m_uuid.c_str(), animation.Get()));

    dstAnimation->AssingnRef(animation);
}

/*
 *
 */

