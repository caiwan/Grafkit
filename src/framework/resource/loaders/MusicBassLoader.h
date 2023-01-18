#pragma once

#include "resource/ResourceBuilder.h"
//#include "core/exceptions.h"
//
//#include "../utils/resource.h"

namespace Grafkit {

	class MusicBassLoader : public IResourceBuilder {

	public:
	    explicit MusicBassLoader(const std::string& source_name);
		~MusicBassLoader();

	    void Load(IResourceManager * const & resman, IResource * source) override;

	    IResource* NewResource() override;
	};

}

DEFINE_EXCEPTION(MusicLoadException, 0, "Could not load music from disk");
DEFINE_EXCEPTION(MusicDeviceInitException, 0, "Could not init playback device");
