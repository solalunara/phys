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
    GlobalTexture( Texture *tex );
    GlobalTexture( const unsigned char *buffer, const char *name, unsigned int width, unsigned int rows );
    ~GlobalTexture();

    Texture *FindLocalTexture( Window *w );

    char *path;

    const int &width = _width;
    const int &height = _height;
private:
    int _width;
    int _height;
};
inline vector<GlobalTexture *> GlobalTextures;

#endif
