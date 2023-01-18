#pragma once

#include "../utils/exceptions.h"

#include "../utils/asset.h"
#include "../utils/resource.h"
#include "../utils/ResourceBuilder.h"

namespace Grafkit {

	class MusicBassLoader : public Grafkit::IResourceBuilder {

	public:
		MusicBassLoader(std::string source_name);
		~MusicBassLoader();

		virtual void Load(IResourceManager * const & resman, IResource * source);

		virtual IResource* NewResource();
	};

}

DEFINE_EXCEPTION(MusicLoadException, 0, "Could not load music from disk");
DEFINE_EXCEPTION(MusicDeviceInitException, 0, "Could not init playback device");
