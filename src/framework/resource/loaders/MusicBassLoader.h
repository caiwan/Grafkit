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

    class MusicBassLoader : public ResourceBuilder<Music, MusicParams>
    {
    public:

        MusicBassLoader() {
        }

        explicit MusicBassLoader(const MusicParams& params)
            : ResourceBuilder<Music, MusicParams>(params) {
        }

        MusicBassLoader(const std::string& name, const std::string& uuid, const MusicParams& params)
            : ResourceBuilder<Music, MusicParams>(name, uuid, params) {
        }


        void Load(IResourceManager* const& resman, IResource* source) override;
        void Initialize(Renderer& render, IResourceManager* const& resman, IResource* source) override;

        PERSISTENT_DECL(MusicBassLoader, 1);

        //protected:
        //void Serialize(Archive& ar) override;
    };
}
