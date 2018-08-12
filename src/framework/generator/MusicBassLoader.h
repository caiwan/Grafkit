#pragma once

#include "../utils/exceptions.h"

#include "../utils/resource.h"
#include "../utils/ResourceBuilder.h"

namespace Grafkit {

	class MusicBassLoader : public Grafkit::IResourceBuilder {

	public:
	    explicit MusicBassLoader(const std::string& source_name);
		~MusicBassLoader();

	    void Load(IResourceManager * const & resman, IResource * source) override;

	    IResource* NewResource() override;
	};

}

DEFINE_EXCEPTION(MusicLoadException, 0, "Could not load music from disk");
DEFINE_EXCEPTION(MusicDeviceInitException, 0, "Could not init playback device");
