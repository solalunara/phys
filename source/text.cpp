#include "text.h"
#include "mesh.h"
#include "window.h"
#include <cstring>

UIText::UIText( const char *text, float x, float y, float scale, vec3 color, Window *container ) :
    UIElement( container, Transform( vec3( x, y, -1 ), glm::identity<quat>(), glm::one<vec3>() ) )
{
    float xnet = 0.f;
    for ( int i = 0; i < strlen( text ); ++i )
    {
        Character ch = Characters[ text[ i ] ];

        float xpos = xnet + ch.Bearing.x * scale;
        float ypos = (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        xnet += w;
        float h = ch.Size.y * scale;

        Meshes.push_back( new CharacterMesh( vec2( -w / 2, -h / 2 ), vec2( w / 2, h / 2 ),
            ch.TextureID->FindLocalTexture( container ), Transform( vec3( xpos, ypos, 0 ), glm::identity<quat>(), glm::one<vec3>() ), container, color, text[ i ], this ) );

        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }
}

GameText::GameText( const char *text, float x, float y, float z, float scale, vec3 color, Window *container ) :
    GameElement( container, Transform( vec3( x, y, z ), glm::identity<quat>(), glm::one<vec3>() ) )
{
    float xnet = 0.f;
    for ( int i = 0; i < strlen( text ); ++i )
    {
        Character ch = Characters[ text[ i ] ];

        float xpos = xnet + ch.Bearing.x * scale;
        float ypos = (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        xnet += w;
        float h = ch.Size.y * scale;

        Meshes.push_back( new CharacterMesh( vec2( -w / 2, -h / 2 ), vec2( w / 2, h / 2 ),
            ch.TextureID->FindLocalTexture( container ), Transform( vec3( xpos, ypos, 0 ), glm::identity<quat>(), glm::one<vec3>() ), container, color, text[ i ], this ) );

        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }
}