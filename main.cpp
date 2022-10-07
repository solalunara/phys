#include <cstdlib>
#include <iostream>
#include <vector> 
#include <map>

using std::vector;
using std::map;

#include "shader.h"
#include "mesh.h"
#include "texture.h"
#include "cube.h"
#include "transform.h"
#include "window.h"
#include "GlobalTexture.h"
#include "text.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>


#include <ft2build.h>
#include FT_FREETYPE_H
FT_Library  library;
FT_Face     face;
FT_Error    error;



enum Settings
{
    NONE = 0,
    KeyRotation = 1 << 0,
};

int main( int argc, const char *argv[] )
{
    int Settings = 0;

    if ( !glfwInit() )
        printf( "Couldn't initialize glfw\n" );


    Window *main = new Window( WindowState::Windowed, 90.f, 1000, 768, "phys" );
    GlobalTexture *universe = new GlobalTexture( "./assets/textures/universe.png" );
    GlobalTexture *dirt = new GlobalTexture( "./assets/textures/dirt.png" );
    GlobalTexture *grass = new GlobalTexture( "./assets/textures/grass.png" );
    GlobalTexture *metal = new GlobalTexture( "./assets/textures/metal.png" );
    for ( int i = 1; i < argc; ++i )
    {
        Window *w = new Window( WindowState::Windowed, 90.f, 200, 152, argv[ i ] );
        new Cube( glm::vec3( -.5f, -.5f, -.5f ), glm::vec3( .5f, .5f, .5f ), Transform( glm::vec3( 0 ), glm::identity<glm::quat>(), glm::vec3( 1 ) ), universe->FindLocalTexture( w ), w );
    }
    Window *popup = new Window( WindowState::Windowed, 90.f, 300, 200, "popup" );

    //freetype init
    {
        error = FT_Init_FreeType( &library );
        if ( error )
            printf( "Error initializing freetype library\n" );
        error = FT_New_Face( library, "/usr/share/fonts/TTF/Hack-Regular.ttf", 0, &face );
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
            GlobalTexture *texture = new GlobalTexture( face->glyph->bitmap.buffer, std::to_string( static_cast<int>( c ) ).c_str(), face->glyph->bitmap.width, face->glyph->bitmap.rows );
            // now store character for later use
            Character character = {
                texture, 
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                static_cast<unsigned int>( face->glyph->advance.x )
            };
            Characters.insert( std::pair<char, Character>( c, character ) );
        }
        FT_Done_Face( face );
        FT_Done_FreeType( library );
    }

    //glfwSetInputMode( MainWindow->ID, GLFW_CURSOR, GLFW_CURSOR_HIDDEN );

    int Major, Minor;
    glGetIntegerv( GL_MAJOR_VERSION, &Major );
    glGetIntegerv( GL_MINOR_VERSION, &Minor );
    printf( "OpenGL version: %i.%i\n", Major, Minor );
    if ( Major < 3 || ( Major == 3 && Minor < 3 ) )
        printf( "OpenGL version not supported. Errors likely. Please update to 3.3\n" );

    UIText *hello = new UIText( "helloo", 0, 0, 1.f, glm::vec3( 1.0f, 1.0f, 1.0f ), main );


    double t = glfwGetTime();
    double dt;

    //set the ground
    for ( int i = -20; i < 20; ++i )
        for ( int j = -20; j < 20; ++j )
            new Cube( glm::vec3( -.5f, -.5f, -.5f ), glm::vec3( .5f, .5f, .5f ), Transform( glm::vec3( i, -1.f, j ), glm::identity<quat>(), glm::one<vec3>() ), new Texture *[] {
                dirt->FindLocalTexture( main ),
                dirt->FindLocalTexture( main ),
                dirt->FindLocalTexture( main ),
                dirt->FindLocalTexture( main ),
                grass->FindLocalTexture( main ),
                dirt->FindLocalTexture( main ),
            }, main );

    new Cube( glm::vec3( -.5f, -.5f, -.5f ), glm::vec3( .5f, .5f, .5f ), Transform( glm::vec3( 0, 0, -1 ), glm::identity<quat>(), glm::one<vec3>() ), metal->FindLocalTexture( main ), main );

    

    while ( true )
    {
        //per frame for all windows
        double t_new = glfwGetTime();
        dt = t_new - t;
        t = t_new;


        //per frame per window
        for ( int i = 0; i < Windows.size(); ++i )
        {
            if ( glfwWindowShouldClose( Windows[ i ]->ID ) )
            {
                delete Windows[ i ];
                if ( Windows.size() )
                    continue;
                else
                {
                    glfwTerminate();
                    exit( 0 );
                }
            }
            glfwMakeContextCurrent( Windows[ i ]->ID );
            glClearColor( .2f, .3f, .3f, 1.0f );
            glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

            if ( Windows[ i ]->GetKeyFlag( GLFW_KEY_ESCAPE ) )
            {
                Windows[ i ]->SetKeyFlag( GLFW_KEY_ESCAPE, false );
                Windows[ i ]->LockCursor = !Windows[ i ]->LockCursor;
                if ( !Windows[ i ]->LockCursor )
                    glfwSetInputMode( Windows[ i ]->ID, GLFW_CURSOR, GLFW_CURSOR_NORMAL );
                else
                    glfwSetInputMode( Windows[ i ]->ID, GLFW_CURSOR, GLFW_CURSOR_HIDDEN );
            }

            if ( Windows[ i ]->GetKeyFlag( GLFW_KEY_W ) )
                Windows[ i ]->CameraTransform.pos += Windows[ i ]->CameraTransform.LocalToWorldDirection( glm::vec3( 0, 0, -dt * 3 ) );
            if ( Windows[ i ]->GetKeyFlag( GLFW_KEY_S ) )
                Windows[ i ]->CameraTransform.pos += Windows[ i ]->CameraTransform.LocalToWorldDirection( glm::vec3( 0, 0, +dt * 3 ) );
            if ( Windows[ i ]->GetKeyFlag( GLFW_KEY_A ) )
                Windows[ i ]->CameraTransform.pos += Windows[ i ]->CameraTransform.LocalToWorldDirection( glm::vec3( -dt * 3, 0, 0 ) );
            if ( Windows[ i ]->GetKeyFlag( GLFW_KEY_D ) )
                Windows[ i ]->CameraTransform.pos += Windows[ i ]->CameraTransform.LocalToWorldDirection( glm::vec3( +dt * 3, 0, 0 ) );
            if ( Windows[ i ]->GetKeyFlag( GLFW_KEY_Q ) )
                Windows[ i ]->CameraTransform.pos += Windows[ i ]->CameraTransform.LocalToWorldDirection( glm::vec3( 0, +dt * 3, 0 ) );
            if ( Windows[ i ]->GetKeyFlag( GLFW_KEY_E ) )
                Windows[ i ]->CameraTransform.pos += Windows[ i ]->CameraTransform.LocalToWorldDirection( glm::vec3( 0, -dt * 3, 0 ) );

            //can't do *= because it does the * in the wrong order (rot * new instaed of new * rot)
            if ( Windows[ i ]->LockCursor )
            {
                if ( Settings & KeyRotation ) //legacy key rotation as opposed to cursor rotation
                {
                    if ( Windows[ i ]->GetKeyFlag( GLFW_KEY_UP ) )
                        Windows[ i ]->CameraTransform.rot = glm::angleAxis( (float)glm::radians( +dt * 90 ), Windows[ i ]->CameraTransform.LocalToWorldDirection( glm::vec3( 1, 0, 0 ) ) ) * Windows[ i ]->CameraTransform.rot;
                    if ( Windows[ i ]->GetKeyFlag( GLFW_KEY_DOWN ) )
                        Windows[ i ]->CameraTransform.rot = glm::angleAxis( (float)glm::radians( -dt * 90 ), Windows[ i ]->CameraTransform.LocalToWorldDirection( glm::vec3( 1, 0, 0 ) ) ) * Windows[ i ]->CameraTransform.rot;
                    if ( Windows[ i ]->GetKeyFlag( GLFW_KEY_LEFT ) )
                        Windows[ i ]->CameraTransform.rot = glm::angleAxis( (float)glm::radians( +dt * 90 ), glm::vec3( 0, 1, 0 ) ) * Windows[ i ]->CameraTransform.rot;
                    if ( Windows[ i ]->GetKeyFlag( GLFW_KEY_RIGHT ) )
                        Windows[ i ]->CameraTransform.rot = glm::angleAxis( (float)glm::radians( -dt * 90 ), glm::vec3( 0, 1, 0 ) ) * Windows[ i ]->CameraTransform.rot;
                }
                else 
                {
                    double xpos, ypos;
                    int xsiz, ysiz;
                    glfwGetCursorPos( Windows[ i ]->ID, &xpos, &ypos );
                    glfwGetFramebufferSize( Windows[ i ]->ID, &xsiz, &ysiz );
                    glfwSetCursorPos( Windows[ i ]->ID, xsiz / 2.f, ysiz / 2.f );
                    double xmv, ymv;
                    xmv = xpos - xsiz / 2;
                    ymv = ypos - ysiz / 2;
                    Windows[ i ]->CameraTransform.rot = glm::angleAxis( (float)glm::radians( -dt * 10 * ymv ), Windows[ i ]->CameraTransform.LocalToWorldDirection( glm::vec3( 1, 0, 0 ) ) ) * Windows[ i ]->CameraTransform.rot;
                    Windows[ i ]->CameraTransform.rot = glm::angleAxis( (float)glm::radians( -dt * 10 * xmv ), glm::vec3( 0, 1, 0 ) ) * Windows[ i ]->CameraTransform.rot;
                }
            }

            if ( Windows[ i ]->GetKeyFlag( GLFW_KEY_P ) )
            {
                Windows[ i ]->SetKeyFlag( GLFW_KEY_P, false );
                /*
                if ( !ent->transform.HasParent() )
                {
                    glm::vec3 pos = ent->transform.GetAbsOrigin();
                    glm::quat rot = ent->transform.GetAbsRot();
                    ent->transform.SetParent( &Windows[ i ]->CameraTransform );
                    ent->transform.SetAbsOrigin( pos );
                    ent->transform.SetAbsRot( rot );
                }
                else
                {
                    glm::vec3 pos = ent->transform.GetAbsOrigin();
                    glm::quat rot = ent->transform.GetAbsRot();
                    ent->transform.SetParent( NULL );
                    ent->transform.SetAbsOrigin( pos );
                    ent->transform.SetAbsRot( rot );
                }
                */
            }

            Windows[ i ]->Render();
            glfwSwapBuffers( Windows[ i ]->ID );
            glfwPollEvents();
        }
    }
}
