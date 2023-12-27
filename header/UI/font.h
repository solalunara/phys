#ifndef FONT_H
#define FONT_H
#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H
#include <glm/glm.hpp>
#include <map>

using glm::ivec2;
using std::map;

struct GlobalTexture;

struct Glyph 
{
    GlobalTexture * TextureID;
    ivec2           Size;
    ivec2           Bearing; // Offset from baseline to left/top of glyph
    unsigned int    Advance; // Offset to advance to next glyph
};

struct Font
{
    Font( const char *FontPath );
    FT_Library  library;
    FT_Face     face;
    FT_Error    error;
    map<char, Glyph> char_to_glyph;
};
static const char *DefaultFont = "/usr/share/fonts/TTF/Hack-Regular.ttf";



#endif