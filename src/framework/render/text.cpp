#include "stdafx.h"
#include "text.h"

#include "texture.h"

using namespace Grafkit;

FontMap::FontMap(): fontsize(0)
    , charset(0)
    , isUnicode(0)
    , strechH(0)
    , isSmooth(0)
    , MSAAlevel(0)
    , lineHeight(0)
    , base(0)
    , maxPages(0)
    , isUseKerning(0) {
}

FontMap::~FontMap() {
}

TextureRef FontMap::GetPage(int n) { return this->pages[n]; }
