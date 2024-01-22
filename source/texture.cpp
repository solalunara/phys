#include "texture.h"
#include "window.h"

#include <png.h>
#include <webp/decode.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <filesystem>
#include <string.h>
#include <system_error>

Texture::Texture( const unsigned char *buffer, const char *name, unsigned int width, unsigned int rows, Window *container ) :
    container( container ), _width( width ), _height( rows )
{
    this->path = new char[ strlen( name ) + 1 ];
    strcpy( this->path, name );
    glfwMakeContextCurrent( container->ID );
    glPixelStorei( GL_UNPACK_ALIGNMENT, 1 ); // disable byte-alignment restriction
    glGenTextures( 1, &_id );
    glBindTexture( GL_TEXTURE_2D, _id );
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_R8,
        width,
        rows,
        0,
        GL_RED,
        GL_UNSIGNED_BYTE,
        buffer
    );
    // set texture options
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    container->Textures.push_back( this );
}

Texture::Texture( const char *path, Window *container ) :
    container( container )
{
    this->path = new char[ strlen( path ) + 1 ];
    strcpy( this->path, path );
    glfwMakeContextCurrent( container->ID );
    container->shader.Use();
    glGenTextures( 1, &_id );
    glBindTexture( GL_TEXTURE_2D, _id );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    std::filesystem::path filesys_path = std::filesystem::path( path );
    const char *ext = filesys_path.extension().c_str();
    if ( !strcmp( ext, ".png" ) )
    {
        FILE *fp = fopen( path, "rb" );
        if ( !fp )
        {
            char *err = new char[ strlen( path ) + 100 ];
            sprintf( err, "Error creating texture: could not open texture %s", path );
            throw std::filesystem::filesystem_error( std::string( err ), filesys_path, std::error_code( std::io_errc::stream ) );
        }
        unsigned char header[ 8 ] { 0 };
        fread( header, 1, 8, fp );
        if ( png_sig_cmp( header, 0, 8 ) )
        {
            char *err = new char[ strlen( path ) + 100 ];
            sprintf( err, "Error creating texture: %s is not a png", path );
            throw std::filesystem::filesystem_error( std::string( err ), filesys_path, std::error_code( std::io_errc::stream ) );
        }

        char error[ 512 ] { 0 };
        png_structp png_ptr = png_create_read_struct( PNG_LIBPNG_VER_STRING, (png_voidp)error, NULL, NULL );
        if ( !png_ptr )
        {
            char *err = new char[ strlen( path ) + 100 ];
            sprintf( err, "Error creating texture: %s", error );
            throw std::filesystem::filesystem_error( std::string( err ), filesys_path, std::error_code( std::io_errc::stream ) );
        }

        png_infop info_ptr = png_create_info_struct( png_ptr );
        if ( !info_ptr )
        {
            png_destroy_read_struct( &png_ptr, (png_infopp)NULL, (png_infopp)NULL );
            fclose( fp );
            const char *err = "Error creating texture: info struct failed to create";
            throw std::filesystem::filesystem_error( std::string( err ), filesys_path, std::error_code( std::io_errc::stream ) );
        }

        png_infop end_info = png_create_info_struct( png_ptr );
        if ( !end_info )
        {
            png_destroy_read_struct( &png_ptr, &info_ptr, (png_infopp)NULL );
            fclose( fp );
            const char *err = "Error creating texture: end info struct failed to create";
            throw std::filesystem::filesystem_error( std::string( err ), filesys_path, std::error_code( std::io_errc::stream ) );
        }

        if ( setjmp( png_jmpbuf( png_ptr ) ) )
        {
            png_destroy_read_struct( &png_ptr, &info_ptr, &end_info );
            fclose( fp );
        }

        png_init_io( png_ptr, fp );
        png_set_sig_bytes( png_ptr, 8 );

        png_read_png( png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL );

        unsigned int w, h;
        int bit_depth, colour_type, interlace_type, compression_type, filter_method;
        png_get_IHDR( png_ptr, info_ptr, &w, &h, &bit_depth, &colour_type, &interlace_type, &compression_type, &filter_method );
        _width = (int)w;
        _height = (int)h;

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
            throw std::system_error( std::error_code( std::io_errc::stream ), "Error: colour mode not supported" );
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
        {
            char *err = new char[ strlen( path ) + 100 ];
            sprintf( err, "Error creating texture: could not open texture %s", path );
            throw std::filesystem::filesystem_error( std::string( err ), filesys_path, std::error_code( std::io_errc::stream ) );
        }
        fseek( fp, 0, SEEK_END );
        long fsize = ftell( fp );
        fseek( fp, 0, SEEK_SET );
        unsigned char raw_data[ fsize ] { 0 };
        fread( raw_data, 1, fsize, fp );
        fclose( fp );

        unsigned char *data = WebPDecodeRGBA( raw_data, fsize, &_width, &_height );
        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data );
        glGenerateMipmap( GL_TEXTURE_2D );
    }
    else throw std::system_error( std::error_code( std::io_errc::stream ), "Error: could not make texture, was not png or webp" );
    container->Textures.push_back( this );
}

Texture *Texture::GenSolidTexture( vec4 colour, glm::ivec2 size, const char *name, Window *container )
{
    unsigned char *data = new unsigned char[ size.x * size.y * 4 ];
    for ( int i = 0; i < size.y; ++i )
        for ( int j = 0; j < size.x; ++j )
            for ( int k = 0; k < 4; ++k )
                data[ ( i * size.x + j ) * 4 + k ] = static_cast<unsigned char>( colour[ k ] );
    return new Texture( data, name, size.x, size.y, container );
}

Texture::~Texture()
{
    delete[] path;
    glDeleteTextures( 1, &_id );
    _id = 0;
}