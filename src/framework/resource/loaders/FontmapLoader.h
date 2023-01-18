#pragma once 

#include "core/exceptions.h"
#include "render/text.h"

namespace Grafkit {

    //class FontMap;
/*
    class BMFontParser {
    public:
        explicit BMFontParser(FontMap *root);
        void load();

    private:
        FontMap * root;
    };*/
}

DEFINE_EXCEPTION(InvalidBMFVersionException, 301, "Invalid BMF version")
