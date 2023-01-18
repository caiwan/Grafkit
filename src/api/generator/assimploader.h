#pragma once

#include "exceptions.h"

#include "../core/renderassets.h"
#include "../render/Scenegraph.h"
#include "../render/model.h"
#include "../render/texture.h"

namespace FWmodel {

	/// @todo legyen resourve tipus, mint regen volt 
	class AssimpLoader : public FWassets::IRenderAssetGenerator
	{
	public:
		AssimpLoader(FWassets::IRenderAssetManager * const &assman, FWrender::Scenegraph * const & scenegraph, const char  * filename);
		AssimpLoader(FWassets::IRenderAssetManager * const &assman, FWrender::Scenegraph * const & scenegraph, const WCHAR * filename);
		AssimpLoader(FWassets::IRenderAssetManager * const &assman, FWrender::Scenegraph * const & scenegraph, const void  * data, size_t length);

		~AssimpLoader();

		virtual void operator () ();

	private:
		struct {
			void * data;
			size_t length;
			int is_file;
		} m_mem;

		FWrender::Scenegraph * const & m_scenegraph;
	};

}

// exceptions
DEFINE_EXCEPTION(AssimpParseException, 4000, "Error while parsing file via ASSIMP")
