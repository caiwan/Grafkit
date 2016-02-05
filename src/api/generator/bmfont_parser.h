#pragma once 

#include "exceptions.h"
#include "../render/text.h"
#include "../render/renderassets.h"

namespace FWtext{

	class FontMap;

	class BMFontParser{
		public:
			BMFontParser(FontMap *root);
			void load();

		private:
			FontMap* root;
	};
}

DEFINE_EXCEPTION(InvalidBMFVersionException, 301, "Invalid BMF version")
