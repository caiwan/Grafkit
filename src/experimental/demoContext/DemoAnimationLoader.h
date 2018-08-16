#pragma once

#include "experimental/AnimationLoader.h"

namespace GkDemo
{
    class DemoAnimationLoader : public Grafkit::IAnimationLoader
    {
    public:
        explicit DemoAnimationLoader(std::string name, std::string sourcename = "", std::string uuid = "", std::string targetUuid = "");

        Grafkit::IResource* NewResource() override;
        void Load(Grafkit::IResourceManager* const& resman, Grafkit::IResource* source) override;
    };
}
