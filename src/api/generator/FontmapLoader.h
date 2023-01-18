#pragma once 

#include "../utils/exceptions.h"
#include "../utils/ResourceBuilder.h"
#include "../render/text.h"


namespace Grafkit{

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
