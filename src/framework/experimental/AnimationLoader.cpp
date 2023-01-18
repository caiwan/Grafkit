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

ActorAnimationLoader::ActorAnimationLoader(std::string name, std::string sourcename, std::string uuid, std::string targetUuid) : IAnimationLoader(name, sourcename, uuid, targetUuid) {
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

        animation->SetName(m_name);
        animation->SetUuid(m_uuid);
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

            animation->SetName(m_name);
            animation->SetUuid(m_uuid);
        }
    }

    if (!animation)
        THROW_EX_DETAILS(NullPointerException, "Could not load Animation");

    LOGGER(Log::Logger().Debug("Animation loaded %s %s Ptr: %p", m_name.c_str(), m_uuid.c_str(), animation.Get()));
    {
        for (size_t i = 0; i < animation->GetTrackCount(); ++i)
        {
            Ref<Animation::Track> track = animation->GetTrack(i);
            LOGGER(Log::Logger().Debug("- Track %s Ptr: %p", track->GetName().c_str(), track.Get()));

            for (size_t j = 0; j < track->GetChannelCount(); ++j)
            {
                Ref<Animation::Channel> channel = track->GetChannel(j);
                LOGGER(Log::Logger().Debug("-- Channel %s Ptr: %p", channel->GetName().c_str(), channel.Get()));
            }
        }
    }

    dstAnimation->AssingnRef(animation);
}

/*
 *
 */

