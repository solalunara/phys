#include "texture.h"

#include <png.h>
#include <webp/decode.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <filesystem>
#include <string.h>

Texture::Texture( const char *path )
{
    glGenTextures( 1, &_id );
    glBindTexture( GL_TEXTURE_2D, _id );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    const char *ext = std::filesystem::path( path ).extension().c_str();
    if ( !strcmp( ext, ".png" ) )
    {
        FILE *fp = fopen( path, "rb" );
        if ( !fp )
            printf( "Error creating texture: could not open texture %s\n", path );
        unsigned char header[ 8 ] { 0 };
        fread( header, 1, 8, fp );
        if ( png_sig_cmp( header, 0, 8 ) )
            printf( "Error creating texture: %s is not a png\n", path );

        char error[ 512 ] { 0 };
        png_structp png_ptr = png_create_read_struct( PNG_LIBPNG_VER_STRING, (png_voidp)error, NULL, NULL );
        if ( !png_ptr )
            printf( "Error creating texture: %s\n", error );

        png_infop info_ptr = png_create_info_struct( png_ptr );
        if ( !info_ptr )
        {
            printf( "Error creating texture: info struct failed to create" );
            png_destroy_read_struct( &png_ptr, (png_infopp)NULL, (png_infopp)NULL );
            fclose( fp );
        }

        png_infop end_info = png_create_info_struct( png_ptr );
        if ( !end_info )
        {
            printf( "Error creating texture: end info struct failed to create" );
            png_destroy_read_struct( &png_ptr, &info_ptr, (png_infopp)NULL );
            fclose( fp );
        }

        if ( setjmp( png_jmpbuf( png_ptr ) ) )
        {
            png_destroy_read_struct( &png_ptr, &info_ptr, &end_info );
            fclose( fp );
        }

        png_init_io( png_ptr, fp );
        png_set_sig_bytes( png_ptr, 8 );

        png_read_png( png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL );

        unsigned int width, height;
        int bit_depth, colour_type, interlace_type, compression_type, filter_method;
        png_get_IHDR( png_ptr, info_ptr, &width, &height, &bit_depth, &colour_type, &interlace_type, &compression_type, &filter_method );

        unsigned char **data_2d = png_get_rows( png_ptr, info_ptr );
        unsigned char data[ height * width * 4 ] { 0 };

        for ( int i = 0; i < height; ++i )
        {
            for ( int j = 0; j < width; ++j )
            {
                for ( int k = 0; k < 4; ++k )
                {
                    data[ ( i * width + j ) * 4 + k ] = data_2d[ i ][ j * 4 + k ];
                }
            }
        }

        unsigned int gl_colour_mode = 0;
        switch ( colour_type )
        {
            case PNG_COLOR_TYPE_RGB:
            gl_colour_mode = GL_RGB;
            break;

            case PNG_COLOR_TYPE_RGB_ALPHA:
            gl_colour_mode = GL_RGBA;
            break;

            case PNG_COLOR_TYPE_GRAY:
            case PNG_COLOR_TYPE_GRAY_ALPHA:
            case PNG_COLOR_TYPE_PALETTE:
            printf( "Error: colour mode not supported" );
            break;
        }

        glTexImage2D( GL_TEXTURE_2D, 0, gl_colour_mode, width, height, 0, gl_colour_mode, GL_UNSIGNED_BYTE, data );
        glGenerateMipmap( GL_TEXTURE_2D );

        png_destroy_read_struct( &png_ptr, &info_ptr, &end_info );
        fclose( fp );
    }
    else if ( !strcmp( ext, ".webp" ) )
    {
        FILE *fp = fopen( path, "r" );
        if ( !fp )
            printf( "Error creating texture: could not open texture %s\n", path );
        fseek( fp, 0, SEEK_END );
        long fsize = ftell( fp );
        fseek( fp, 0, SEEK_SET );
        unsigned char raw_data[ fsize ] { 0 };
        fread( raw_data, 1, fsize, fp );
        fclose( fp );

        int width, height;
        unsigned char *data = WebPDecodeRGBA( raw_data, fsize, &width, &height );
        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data );
        glGenerateMipmap( GL_TEXTURE_2D );
    }
}

Texture::~Texture()
{
    glDeleteTextures( 1, &_id );
    _id = 0;
}