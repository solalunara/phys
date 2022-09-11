#ifndef TEXTURE_H
#define TEXTURE_H

#include <cstring>

#pragma once

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

    char path[ 512 ];
    const unsigned int &id = _id;

    Window *container;

private:
    unsigned int _id;
};

#endif