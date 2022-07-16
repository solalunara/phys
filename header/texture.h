#ifndef TEXTURE_H
#define TEXTURE_H

#pragma once

struct Texture
{
    Texture( const char *path );
    ~Texture();

    Texture( const Texture & ) = delete;
    Texture &operator =( const Texture & ) = delete;

    Texture( Texture &&other ) : _id( other._id ), path( other.path )
    {
        other._id = 0;
    }

    const char *path;
    const unsigned int &id = _id;

private:
    unsigned int _id;
};

#endif