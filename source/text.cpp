#include "text.h"
#include "mesh.h"
#include "window.h"
#include <cstring>

UIText::UIText( const char *text, float x, float y, float scale, vec3 color, Window *container ) :
    UIElement( container, Transform( vec3( x, y, -1 ), glm::identity<quat>(), glm::one<vec3>() ) )
{
    unsigned long long len = strlen( text );
    struct Data
    {
        Character ch;
        float xpos;
        float ypos;
        float w;
        float h;
    };
    Data data[ len ] {};

    float TextWidth = 0;

    float xnet = 0.f;
    for ( unsigned long long i = 0; i < len; ++i )
    {
        Character ch = Characters[ text[ i ] ];

        float xpos = xnet + ch.Bearing.x * scale;
        float ypos = (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;

        data[ i ] = Data{ ch, xpos, ypos, w, h };

        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        if ( i + 1 < len )
            xnet += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }

    TextWidth = xnet;

    for ( unsigned long long i = 0; i < len; ++i )
    {
        Meshes.push_back( new CharacterMesh( vec2( -data[ i ].w / 2, -data[ i ].h / 2 ), vec2( data[ i ].w / 2, data[ i ].h / 2 ),
            data[ i ].ch.TextureID->FindLocalTexture( container ), Transform( vec3( data[ i ].xpos - TextWidth / 2, data[ i ].ypos, 0 ), glm::identity<quat>(), glm::one<vec3>() ), 
            container, color, text[ i ], this ) );
    }
}

GameText::GameText( const char *text, float x, float y, float z, float scale, vec3 color, Window *container ) :
    GameElement( container, Transform( vec3( x, y, z ), glm::identity<quat>(), glm::one<vec3>() ) )
{
    unsigned long long len = strlen( text );
    struct Data
    {
        Character ch;
        float xpos;
        float ypos;
        float w;
        float h;
    };
    Data data[ len ] {};

    float TextWidth = 0;

    float xnet = 0.f;
    for ( int i = 0; i < strlen( text ); ++i )
    {
        Character ch = Characters[ text[ i ] ];

        float xpos = xnet + ch.Bearing.x * scale;
        float ypos = (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;

        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        if ( i + 1 < len )
            xnet += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }

    TextWidth = xnet;

    for ( unsigned long long i = 0; i < len; ++i )
    {
        Meshes.push_back( new CharacterMesh( vec2( -data[ i ].w / 2, -data[ i ].h / 2 ), vec2( data[ i ].w / 2, data[ i ].h / 2 ),
            data[ i ].ch.TextureID->FindLocalTexture( container ), Transform( vec3( data[ i ].xpos - TextWidth / 2, data[ i ].ypos, 0 ), glm::identity<quat>(), glm::one<vec3>() ), 
            container, color, text[ i ], this ) );
    }
}