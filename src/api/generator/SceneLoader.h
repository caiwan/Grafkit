#pragma once

#include "utils/exceptions.h"

#include "utils/asset.h"
#include "utils/resource.h"
#include "utils/ResourceBuilder.h"
			
#include "render/Scene.h"
#include "render/model.h"
#include "render/texture.h"

namespace Grafkit {

	class SceneLoader : public Grafkit::IResourceBuilder
	{
	public:
		SceneLoader(std::string name, std::string source_name);
		~SceneLoader();

		void Load(Grafkit::IResourceManager * const & assman, Grafkit::IResource * source);

		static void Save(SceneRes scene, std::string dst_name);

		virtual IResource* NewResource() { return new SceneRes(); }

	};
}

// exceptions
DEFINE_EXCEPTION(SceneLoadException, 4000, "Error deserialize scene data");
