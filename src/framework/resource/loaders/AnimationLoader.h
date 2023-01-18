#pragma once
#include "core/resource.h"
#include "resource/ResourceBuilder.h"

namespace Grafkit
{
    class IAnimationLoader : public IResourceBuilder
    {
    public:
        explicit IAnimationLoader(std::string name, std::string sourcename = "", std::string uuid = "", std::string targetUuid = "")
            : IResourceBuilder(name, sourcename, uuid)
            , m_target(targetUuid) {
        }

    protected:
        std::string m_target;
    };

    class ActorAnimationLoader : public IAnimationLoader
    {
    public:
        explicit ActorAnimationLoader(std::string name, std::string sourcename = "", std::string uuid = "", std::string targetUuid = "");

        IResource* NewResource() override;
        void Load(IResourceManager* const& resman, IResource* source) override;
    };
}

