#pragma once

#include "resource/ResourceBuilder.h"

namespace Grafkit
{
    struct MusicParams
    {
        std::string source;

        template <class A>
        void Serialize(A& ar) { ar & source; }
    };

    class MusicBassLoader : public ResourceLoader<Music, MusicParams>
    {
    public:


        MusicBassLoader(const Uuid& id, const MusicParams& params)
            : ResourceLoader<Music, MusicParams>(id, params) {
        }

    protected:
        void Load(const std::shared_ptr<IResource>& resource, ResourceManager& resourceManager, const IAssetFactory & assetFactory) override;
        void Initialize(const std::shared_ptr<IResource>& resource, ResourceManager& resourceManager, const IAssetFactory & assetFactory) override;
    };
}
