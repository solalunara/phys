#ifndef GLOBALTEXTURE_H
#define GLOBALTEXTURE_H

#pragma once

#include <vector>
using std::vector;

struct Texture;
struct Window;

class GlobalTexture
{
public:
    GlobalTexture( const char *path );
    GlobalTexture( const unsigned char *buffer, const char *name, unsigned int width, unsigned int rows );
    ~GlobalTexture();

    Texture *FindLocalTexture( Window *w );

    char *path;
};
inline vector<GlobalTexture *> GlobalTextures;

#endif
