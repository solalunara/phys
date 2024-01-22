#include "text.h"
#include "mesh.h"
#include "window.h"
#include "UI/font.h"
#include <cstring>
#include <stdexcept>

UIText::UIText( const char *text, Font *font, vec3 pos, float scale, vec3 color, Window *container ) :
    UIElement( container, new Transform( pos, glm::identity<quat>(), glm::one<vec3>() ) )
{
    if ( pos.z >= 0 )
        throw std::invalid_argument( "pos.z must be <0" );
    bool InitialFontNull = !font;
    if ( InitialFontNull )
        font = new Font( DefaultFont );

    unsigned long long len = strlen( text );
    struct Data
    {
        Glyph ch;
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
        Glyph ch = font->char_to_glyph[ text[ i ] ];

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
        AddElement( new CharacterMesh( vec2( -data[ i ].w / 2, -data[ i ].h / 2 ), vec2( data[ i ].w / 2, data[ i ].h / 2 ),
            data[ i ].ch.TextureID->FindLocalTexture( container ), new Transform( vec3( data[ i ].xpos - TextWidth / 2, data[ i ].ypos, 0 ), glm::identity<quat>(), glm::one<vec3>() ), 
            container, color, text[ i ], this ) );
    }

    if ( InitialFontNull )
        delete font;
}

GameText::GameText( const char *text, Font *font, vec3 pos, float scale, vec3 color, Window *container ) :
    GameElement( container, new Transform( pos, glm::identity<quat>(), glm::one<vec3>() ) )
{    
    bool InitialFontNull = !font;
    if ( InitialFontNull )
        font = new Font( DefaultFont );


    unsigned long long len = strlen( text );
    struct Data
    {
        Glyph ch;
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
        Glyph ch = font->char_to_glyph[ text[ i ] ];

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
        AddElement( new CharacterMesh( vec2( -data[ i ].w / 2, -data[ i ].h / 2 ), vec2( data[ i ].w / 2, data[ i ].h / 2 ),
            data[ i ].ch.TextureID->FindLocalTexture( container ), new Transform( vec3( data[ i ].xpos - TextWidth / 2, data[ i ].ypos, 0 ), glm::identity<quat>(), glm::one<vec3>() ), 
            container, color, text[ i ], this ) );
    }


    if ( InitialFontNull )
        delete font;
}