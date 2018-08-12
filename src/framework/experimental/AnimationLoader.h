#pragma once 
#include <map>
#include <vector>

#include "utils/exceptions.h"

#include "utils/persistence/persistence.h"

#include "utils/asset.h"
#include "utils/resource.h"
#include "utils/ResourceBuilder.h"


namespace Grafkit
{

    class ActorAnimationLoader : public IResourceBuilder
    {
    public:
        explicit ActorAnimationLoader(std::string name, std::string sourcename = "", std::string uuid = "")
            : IResourceBuilder(name, sourcename, uuid) {
        }

        IResource* NewResource() override;
        void Load(IResourceManager* const& resman, IResource* source) override;
    };

}


