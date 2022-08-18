#include <cstdlib>
#include <iostream>
#include <vector> 

#include "shader.h"
#include "mesh.h"
#include "texture.h"
#include "entity.h"
#include "transform.h"
#include "window.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>

using std::vector;


static void KeyCallback( GLFWwindow *window, int key, int scancode, int action, int mods );

long KeyFlags[ 6 ] { 0 };
static void SetKeyFlag( int key, bool set );
static bool GetKeyFlag( int key );

enum Settings
{
    NONE = 0,
    LockCursor = 1 << 0,
    KeyRotation = 1 << 1,
};

int main( int argc, const char *argv[] )
{
    int Settings;
    Settings |= LockCursor;

    if ( !glfwInit() )
        printf( "Couldn't initialize glfw\n" );


    Window *main = new Window( WindowState::Windowed, 1000, 768, "phys" );
    Window *popup = new Window( WindowState::Windowed, 100, 76, "popup" );
    if ( !MainWindow )
    {
        const char *msg[ 1 ] { new char[ 1024 ] { 0 } };
        glfwGetError( msg );
        printf( "Couldn't create window - %s\n", msg[ 0 ] );
        printf( "Possible restart needed after driver update\n" );
    }
    glfwMakeContextCurrent( MainWindow->ID );
    glfwSetKeyCallback( MainWindow->ID, KeyCallback );

    glfwSetInputMode( MainWindow->ID, GLFW_CURSOR, GLFW_CURSOR_HIDDEN );

    int Major, Minor;
    glGetIntegerv( GL_MAJOR_VERSION, &Major );
    glGetIntegerv( GL_MINOR_VERSION, &Minor );
    printf( "OpenGL version: %i.%i\n", Major, Minor );
    if ( Major < 3 || ( Major == 3 && Minor < 3 ) )
        printf( "OpenGL version not supported. Errors likely. Please update to 3.3\n" );

    glEnable( GL_DEPTH_TEST );
	glEnable( GL_FRAMEBUFFER_SRGB );
	//glEnable( GL_CULL_FACE );
	glDepthFunc( GL_LESS );

    Texture *universe = new Texture( "./assets/textures/universe.png", MainWindow );
    Texture *universe_popup = new Texture( "./assets/textures/universe.png", popup );

    Entity *ent = new Entity( glm::vec3( -.5f, -.5f, -.5f ), glm::vec3( .5f, .5f, .5f ), Transform( glm::vec3( 0 ), glm::identity<glm::quat>(), glm::vec3( 1 ) ), universe, MainWindow );
    Entity *ent_popup = new Entity( glm::vec3( -.5f, -.5f, -.5f ), glm::vec3( .5f, .5f, .5f ), Transform( glm::vec3( 0 ), glm::identity<glm::quat>(), glm::vec3( 1 ) ), universe_popup, popup );
    Entity *bbox = new Entity( glm::vec3( -5.f, -5.f, -5.f ), glm::vec3( 5.f, 5.f, 5.f ), Transform( glm::vec3( 0 ), glm::identity<glm::quat>(), glm::vec3( 1 ) ), universe, MainWindow );

    Transform CameraTransform( glm::zero<glm::vec3>(), glm::identity<glm::quat>(), glm::one<glm::vec3>() );
    double t = glfwGetTime();
    while ( !glfwWindowShouldClose( MainWindow->ID ) )
    {
        //per frame for all windows
        double t_new = glfwGetTime();
        double dt = t_new - t;
        t = t_new;

        if ( GetKeyFlag( GLFW_KEY_W ) )
            CameraTransform.pos += CameraTransform.LocalToWorldDirection( glm::vec3( 0, 0, -dt * 3 ) );
        if ( GetKeyFlag( GLFW_KEY_S ) )
            CameraTransform.pos += CameraTransform.LocalToWorldDirection( glm::vec3( 0, 0, +dt * 3 ) );
        if ( GetKeyFlag( GLFW_KEY_A ) )
            CameraTransform.pos += CameraTransform.LocalToWorldDirection( glm::vec3( -dt * 3, 0, 0 ) );
        if ( GetKeyFlag( GLFW_KEY_D ) )
            CameraTransform.pos += CameraTransform.LocalToWorldDirection( glm::vec3( +dt * 3, 0, 0 ) );
        if ( GetKeyFlag( GLFW_KEY_Q ) )
            CameraTransform.pos += CameraTransform.LocalToWorldDirection( glm::vec3( 0, +dt * 3, 0 ) );
        if ( GetKeyFlag( GLFW_KEY_E ) )
            CameraTransform.pos += CameraTransform.LocalToWorldDirection( glm::vec3( 0, -dt * 3, 0 ) );

        //can't do *= because it does the * in the wrong order (rot * new instaed of new * rot)
        if ( Settings & KeyRotation )
        {
            if ( GetKeyFlag( GLFW_KEY_UP ) )
                CameraTransform.rot = glm::angleAxis( (float)glm::radians( +dt * 90 ), CameraTransform.LocalToWorldDirection( glm::vec3( 1, 0, 0 ) ) ) * CameraTransform.rot;
            if ( GetKeyFlag( GLFW_KEY_DOWN ) )
                CameraTransform.rot = glm::angleAxis( (float)glm::radians( -dt * 90 ), CameraTransform.LocalToWorldDirection( glm::vec3( 1, 0, 0 ) ) ) * CameraTransform.rot;
            if ( GetKeyFlag( GLFW_KEY_LEFT ) )
                CameraTransform.rot = glm::angleAxis( (float)glm::radians( +dt * 90 ), glm::vec3( 0, 1, 0 ) ) * CameraTransform.rot;
            if ( GetKeyFlag( GLFW_KEY_RIGHT ) )
                CameraTransform.rot = glm::angleAxis( (float)glm::radians( -dt * 90 ), glm::vec3( 0, 1, 0 ) ) * CameraTransform.rot;
        }
        else if ( Settings & LockCursor ) //only do if not doing key rotation and locking cursor
        {
            double xpos, ypos;
            int xsiz, ysiz;
            glfwGetCursorPos( MainWindow->ID, &xpos, &ypos );
            glfwGetFramebufferSize( MainWindow->ID, &xsiz, &ysiz );
            glfwSetCursorPos( MainWindow->ID, xsiz / 2.f, ysiz / 2.f );
            double xmv, ymv;
            xmv = xpos - xsiz / 2;
            ymv = ypos - ysiz / 2;
            CameraTransform.rot = glm::angleAxis( (float)glm::radians( -dt * 10 * ymv ), CameraTransform.LocalToWorldDirection( glm::vec3( 1, 0, 0 ) ) ) * CameraTransform.rot;
            CameraTransform.rot = glm::angleAxis( (float)glm::radians( -dt * 10 * xmv ), glm::vec3( 0, 1, 0 ) ) * CameraTransform.rot;
        }

        if ( GetKeyFlag( GLFW_KEY_ESCAPE ) )
        {
            SetKeyFlag( GLFW_KEY_ESCAPE, false );
            if ( Settings & LockCursor )
            {
                glfwSetInputMode( MainWindow->ID, GLFW_CURSOR, GLFW_CURSOR_NORMAL );
                Settings &= ~LockCursor;
            }
            else
            {
                glfwSetInputMode( MainWindow->ID, GLFW_CURSOR, GLFW_CURSOR_HIDDEN );
                Settings |= LockCursor;
            }
        }

        if ( GetKeyFlag( GLFW_KEY_P ) )
        {
            SetKeyFlag( GLFW_KEY_P, false );
            if ( !ent->transform.HasParent() )
            {
                glm::vec3 pos = ent->transform.GetAbsOrigin();
                glm::quat rot = ent->transform.GetAbsRot();
                ent->transform.SetParent( &CameraTransform );
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
        }

        //per frame per window
        for ( int i = 0; i < Windows.size(); ++i )
        {
            glfwMakeContextCurrent( Windows[ i ]->ID );
            glClearColor( .2f, .3f, .3f, 1.0f );
            glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
            Windows[ i ]->Render();
            glfwSwapBuffers( Windows[ i ]->ID );
        }
        

        ent->transform.rot *= glm::angleAxis( (float)glm::radians( dt * 90 ), glm::vec3( 0, 1.f, 0 ) );

        glfwPollEvents();
    }

    glfwTerminate();
}

static void KeyCallback( GLFWwindow *window, int key, int scancode, int action, int mods )
{
    switch ( action )
    {
        case GLFW_PRESS:
        SetKeyFlag( key, true );
        break;

        case GLFW_RELEASE:
        SetKeyFlag( key, false );
        break;
    }
}
static void SetKeyFlag( int key, bool set )
{
    if ( set )
        KeyFlags[ key / 64 ] |= 1 << ( key % 64 );
    else
        KeyFlags[ key / 64 ] &= ~( 1 << ( key % 64 ) );
}
static bool GetKeyFlag( int key )
{
    return KeyFlags[ key / 64 ] & ( 1 << ( key % 64 ) );
}