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

        template <class A>
        void Serialize(A& ar) { ar & sourceName & target; }
    };

    class ActorAnimationLoader : public ResourceLoader<ActorAnimation, ActorAnimationParams>
    {
    public:

        ActorAnimationLoader(const std::string& id, const ActorAnimationParams& params)
            : ResourceLoader<ActorAnimation, ActorAnimationParams>(id, params) {
        }

    protected:
        void Load(const std::shared_ptr<IResource>& resource, ResourceManager& resourceManager) override;
        void Initialize(const std::shared_ptr<IResource>& resource, ResourceManager& resourceManager) override;
    };
}
