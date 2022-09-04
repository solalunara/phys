#ifndef TEXTURE_H
#define TEXTURE_H

#pragma once

struct Window;

struct Texture
{
    Texture( const char *path, Window *container );
    Texture( const unsigned char *buffer, const char *name, unsigned int width, unsigned int rows, Window *container );
    ~Texture();

    Texture( const Texture & ) = delete;
    Texture &operator =( const Texture & ) = delete;

    Texture( Texture &&other ) : _id( other._id ), path( other.path )
    {
        other._id = 0;
    }

    const char *path;
    const unsigned int &id = _id;

    Window *container;

private:
    unsigned int _id;
};

#endif