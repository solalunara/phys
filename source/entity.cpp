#include "entity.h"
#include "window.h"
#include "GlobalTexture.h"
#include <string.h>

Entity::Entity( vec3 mins, vec3 maxs, Transform transform, Texture *texture, Window *container ) :
    Entity( mins, maxs, std::move( transform ), new Texture *[ 6 ] { texture, texture, texture, texture, texture, texture }, container )
{
}
Entity::Entity( vec3 mins, vec3 maxs, Transform transform, Texture *textures[ 6 ], Window *container ) :
    sides{ 
        new Mesh( vec2( mins.x, mins.y ), vec2( maxs.x, maxs.y ), textures[ 0 ], Transform( vec3( 0, 0, maxs.z ), glm::identity<quat>(), glm::one<vec3>() ), container, true ),
        new Mesh( vec2( mins.x, mins.y ), vec2( maxs.x, maxs.y ), textures[ 1 ], Transform( vec3( 0, 0, mins.z ), glm::identity<quat>(), glm::one<vec3>() ), container, true ),
        new Mesh( vec2( mins.z, mins.y ), vec2( maxs.z, maxs.y ), textures[ 2 ], Transform( vec3( mins.x, 0, 0 ), glm::angleAxis( M_PI_2f, vec3( 0, 1, 0 ) ), glm::one<vec3>() ), container, true ),
        new Mesh( vec2( mins.z, mins.y ), vec2( maxs.z, maxs.y ), textures[ 3 ], Transform( vec3( maxs.x, 0, 0 ), glm::angleAxis( M_PI_2f, vec3( 0, 1, 0 ) ), glm::one<vec3>() ), container, true ),
        new Mesh( vec2( mins.z, mins.x ), vec2( maxs.z, maxs.x ), textures[ 4 ], Transform( vec3( 0, maxs.y, 0 ), glm::angleAxis( M_PI_2f, vec3( 1, 0, 0 ) ), glm::one<vec3>() ), container, true ),
        new Mesh( vec2( mins.z, mins.x ), vec2( maxs.z, maxs.x ), textures[ 5 ], Transform( vec3( 0, mins.y, 0 ), glm::angleAxis( M_PI_2f, vec3( 1, 0, 0 ) ), glm::one<vec3>() ), container, true ),
    }, transform( std::move( transform ) ), container( container )
{
    for ( int i = 0; i < 6; ++i )
        this->sides[ i ]->transform.SetParent( &this->transform );
    container->Entities.push_back( this );
}

Entity::~Entity()
{
    for ( int i = 0; i < 6; ++i )
        delete sides[ i ];
}

void Entity::Render()
{
    for ( int i = 0; i < 6; ++i )
        sides[ i ]->Render();
}

Word::Word( const char *text, float x, float y, float z, float scale, vec3 color, bool UI, Window *container ) :
    container( container ), UI( UI )
{
    for ( int i = 0; i < strlen( text ); ++i )
    {
        Character ch = Characters[ text[ i ] ];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;

        chars.push_back( new Text( vec2( -w / 2, -h / 2 ), vec2( w / 2, h / 2 ),
            ch.TextureID->FindLocalTexture( container ), Transform( vec3( xpos, ypos, z ), glm::identity<quat>(), glm::one<vec3>() ), container, color, text[ i ], UI, this ) );

        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }
}

Word::~Word()
{
    for ( int i = chars.size(); i >= 0; --i )
    {
        for ( int j = container->Meshes.size(); j >= 0; --j )
        {
            if ( chars[ i ] == container->Meshes[ j ] )
            {
                delete chars[ i ];
                chars.erase( chars.begin() + i );
                container->Meshes.erase( container->Meshes.begin() + j );
                break;
            }
        }
    }
}