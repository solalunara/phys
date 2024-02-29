#include "UI/font.h"
#include "GlobalTexture.h"
#include <string>

using std::to_string;
using std::pair;

Font::Font( const char *FontPath )
{
    error = FT_Init_FreeType( &library );
    if ( error )
        printf( "Error initializing freetype library\n" );

    error = FT_New_Face( library, FontPath, 0, &face );
    if ( error == FT_Err_Unknown_File_Format )
        printf( "Error initializing freetype face -- unknown file format\n" );
    else if ( error )
        printf( "Error initializing freetype face\n" );

    FT_Set_Pixel_Sizes( face, 0, 48 );  
    for ( unsigned char c = 0; c < 128; c++ )
    {
        unsigned int in = FT_Get_Char_Index( face, c );
        FT_Load_Glyph( face, in, FT_LOAD_DEFAULT );
        FT_Render_Glyph( face->glyph, FT_RENDER_MODE_NORMAL );
        // generate texture
        GlobalTexture *texture = new GlobalTexture( face->glyph->bitmap.buffer, to_string( static_cast<int>( c ) ).c_str(), face->glyph->bitmap.width, face->glyph->bitmap.rows );
        // now store character for later use
        Glyph glyph = {
            texture, 
            ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<unsigned int>( face->glyph->advance.x )
        };
        char_to_glyph.insert( pair<char, Glyph>( c, glyph ) );
    }
    FT_Done_Face( face );
    FT_Done_FreeType( library );
}