#include "GlobalTexture.h"
#include "window.h"
#include "texture.h"
#include <string.h>

GlobalTexture::GlobalTexture( const char *path )
{
    this->path = new char[ strlen( path ) + 1 ];
    strcpy( this->path, path );
    for ( int i = 0; i < Windows.size(); ++i )
        if ( !FindLocalTexture( Windows[ i ] ) ) 
            new Texture( path, Windows[ i ] );
    GlobalTextures.push_back( this );
}
GlobalTexture::GlobalTexture( const unsigned char *buffer, const char *name, unsigned int width, unsigned int rows )
{
    this->path = new char[ strlen( name ) + 1 ];
    strcpy( this->path, name );
    for ( int i = 0; i < Windows.size(); ++i )
        if ( !FindLocalTexture( Windows[ i ] ) ) 
            new Texture( buffer, name, width, rows, Windows[ i ] );
    GlobalTextures.push_back( this );
}

GlobalTexture::~GlobalTexture()
{
    for ( int i = 0; i < GlobalTextures.size(); ++i )
    {
        if ( GlobalTextures[ i ] == this )
        {
            GlobalTextures.erase( GlobalTextures.begin() + i );
            break;
        }
    }
    delete[] path;
}

Texture *GlobalTexture::FindLocalTexture( Window *w )
{
    for ( int i = w->Textures.size(); --i >= 0; )
        if ( !strcmp( w->Textures[ i ]->path, path ) ) 
            return w->Textures[ i ];
    return NULL;
}