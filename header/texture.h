#ifndef TEXTURE_H
#define TEXTURE_H

#include <cstring>
#include <glm/glm.hpp>

#pragma once

using glm::vec4;
using glm::ivec2;

struct Window;

struct Texture
{
    Texture( const char *path, Window *container );
    Texture( const unsigned char *buffer, const char *name, unsigned int width, unsigned int rows, Window *container );
    ~Texture();

    Texture( const Texture & ) = delete;
    Texture &operator =( const Texture & ) = delete;

    Texture( Texture &&other ) : _id( other._id )
    {
        strcpy( this->path, path );
        other._id = 0;
    }

    char *path;
    const unsigned int &id = _id;
    const int &width = _width;
    const int &height = _height;

    Window *container;

    static Texture *GenSolidTexture( vec4 colour, ivec2 size, const char *name, Window *container );

private:
    unsigned int _id;

    int _width;
    int _height;
};

#endif