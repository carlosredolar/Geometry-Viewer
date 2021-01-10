#pragma once

#include "Libs/Glew/include/glew.h"
#include "Libs/FreeType/include/ft2build.h"
#include "Libs/FreeType/include/freetype/freetype.h"

class GLFont {
public:
    GLFont(const char* fontFile);
    ~GLFont();

    void setFontFile(const char* fontFile);

    FT_Face getFaceHandle();

private:
    char* _fontFile;
    FT_Error _error;
    FT_Library _ft;
    FT_Face _face;

};

