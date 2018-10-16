#pragma once
#include "core/resource.h"
#include "resource/ResourceBuilder.h"

namespace Grafkit
{
    class ActorAnimation;

    struct ActorAnimationParams
    {
        std::string sourceName;
        std::string target;
    };

    class ActorAnimationLoader : public ResourceBuilder<ActorAnimation, ActorAnimationParams>
    {
    public:
        ActorAnimationLoader() {
        }

        explicit ActorAnimationLoader(const ActorAnimationParams& params)
            : ResourceBuilder<ActorAnimation, ActorAnimationParams>(params) {
        }

        ActorAnimationLoader(const std::string& name, const std::string& uuid, const ActorAnimationParams& params)
            : ResourceBuilder<ActorAnimation, ActorAnimationParams>(name, uuid, params) {
        }

        void Load(IResourceManager* const& resman, IResource* source) override;
        void Initialize(Renderer& render, IResourceManager* const& resman, IResource* source) override;

        PERSISTENT_DECL(ActorAnimationLoader, 1);

    //protected:
    //    void Serialize(Archive& ar) override;
    };
}

