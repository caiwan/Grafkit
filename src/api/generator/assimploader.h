#pragma once

#include "exceptions.h"

#include "../core/renderassets.h"
#include "../render/Scene.h"
#include "../render/model.h"
#include "../render/texture.h"

namespace Grafkit {

	class AssimpLoader : public Grafkit::IRenderAssetBuilder
	{
	public:
		AssimpLoader(Grafkit::IResourceRef resource, Grafkit::Scene * const & scenegraph);
		~AssimpLoader();

		virtual void operator () (Grafkit::IRenderAssetManager * const &assman);

	private:
		Grafkit::IResourceRef m_resource;
		Grafkit::Scene * const & m_scenegraph;
	};

}

// exceptions
DEFINE_EXCEPTION(AssimpParseException, 4000, "Error while parsing file via ASSIMP");
