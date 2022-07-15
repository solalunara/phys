#include <cstdlib>
#include <iostream>

#include "shader.h"
#include "mesh.h"
#include "texture.h"
#include "entity.h"
#include "transform.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>

static void KeyCallback( GLFWwindow *window, int key, int scancode, int action, int mods );
static void ResizeCallback( GLFWwindow *window, int width, int height );

long KeyFlags[ 6 ] { 0 };
static void SetKeyFlag( int key, bool set );
static bool GetKeyFlag( int key );

enum Settings
{
    NONE = 0,
    LockCursor = 1 << 0,
    KeyRotation = 1 << 1,
};

Shader *shader = NULL;
int WindowWidth, WindowHeight;

int main( int argc, const char *argv[] )
{
    int Settings;
    Settings |= LockCursor;

    if ( !glfwInit() )
        printf( "Couldn't initialize glfw\n" );

    GLFWwindow *window = glfwCreateWindow( 1000, 768, "phys", NULL, NULL );
    if ( !window )
    {
        const char *msg[ 10 ] { new char[ 1024 ] { 0 } };
        glfwGetError( msg );
        printf( "Couldn't create window - %s\n", msg[ 0 ] );
        int i = 1;
        while ( msg[ i ][ 0 ] )
        {
            printf( "%s\n", msg[ i ] );
        }
        printf( "Possible restart needed after driver update\n" );
    }
    glfwMakeContextCurrent( window );
    glfwSetFramebufferSizeCallback( window, ResizeCallback );
    glfwSetKeyCallback( window, KeyCallback );

    glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN );

    if ( !gladLoadGLLoader( (GLADloadproc)glfwGetProcAddress ) )
        printf( "Couldn't initialize glad\n" );

    int Major, Minor;
    glGetIntegerv( GL_MAJOR_VERSION, &Major );
    glGetIntegerv( GL_MINOR_VERSION, &Minor );
    printf( "OpenGL version: %i.%i\n", Major, Minor );
    if ( Major < 3 || ( Major == 3 && Minor < 3 ) )
        printf( "OpenGL version not supported. Errors likely. Please update to 3.3\n" );

    glfwGetFramebufferSize( window, &WindowWidth, &WindowHeight );
    glViewport( 0, 0, WindowWidth, WindowHeight );

    glfwSwapInterval( 1 );

    glEnable( GL_DEPTH_TEST );
	glEnable( GL_FRAMEBUFFER_SRGB );
	//glEnable( GL_CULL_FACE );
	glDepthFunc( GL_LESS );


    shader = new Shader();
    shader->Use();
    shader->SetShaderValue( "Perspective", glm::perspectiveFov<float>( (float)glm::radians( 90.f ), WindowWidth, WindowHeight, 0.0001f, 1000.f ) );
    Texture *universe = new Texture( "./assets/textures/universe.png" );


    Mesh *floor = new Mesh( glm::vec2( -1.f, -1.f ), glm::vec2( 1.f, 1.f ), universe, Transform( glm::vec3( 0, -1, 0 ), glm::angleAxis( glm::radians( 90.f ), glm::vec3( 1, 0, 0 ) ), glm::one<glm::vec3>() ) );
    Entity *ent = new Entity( glm::vec3( -.5f, -.5f, -.5f ), glm::vec3( .5f, .5f, .5f ), Transform( glm::vec3( 0 ), glm::identity<glm::quat>(), glm::vec3( 1 ) ), universe );

    Transform CameraTransform( glm::zero<glm::vec3>(), glm::identity<glm::quat>(), glm::one<glm::vec3>() );
    double t = glfwGetTime();
    int frames = 0;
    while ( !glfwWindowShouldClose( window ) )
    {
        double t_new = glfwGetTime();
        double dt = t_new - t;
        t = t_new;

        glClearColor( .2f, .3f, .3f, 1.0f );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );


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
            glfwGetCursorPos( window, &xpos, &ypos );
            glfwSetCursorPos( window, WindowWidth / 2.f, WindowHeight / 2.f );
            double xmv, ymv;
            xmv = xpos - WindowWidth / 2;
            ymv = ypos - WindowHeight / 2;
            CameraTransform.rot = glm::angleAxis( (float)glm::radians( -dt * 10 * ymv ), CameraTransform.LocalToWorldDirection( glm::vec3( 1, 0, 0 ) ) ) * CameraTransform.rot;
            CameraTransform.rot = glm::angleAxis( (float)glm::radians( -dt * 10 * xmv ), glm::vec3( 0, 1, 0 ) ) * CameraTransform.rot;
        }

        if ( GetKeyFlag( GLFW_KEY_ESCAPE ) )
        {
            SetKeyFlag( GLFW_KEY_ESCAPE, false );
            if ( Settings & LockCursor )
            {
                glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_NORMAL );
                Settings &= ~LockCursor;
            }
            else
            {
                glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN );
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
        
        shader->SetShaderValue( "CameraTransform", CameraTransform.GetInverseMatrix() );

        ent->Render( shader );
        ent->transform.rot *= glm::angleAxis( (float)glm::radians( dt * 90 ), glm::vec3( 0, 1.f, 0 ) );

        floor->Render( shader );

        glfwSwapBuffers( window );
        glfwPollEvents();
        ++frames;
    }

    glfwDestroyWindow( window );

    delete ent;
    delete universe;
    delete shader;
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
static void ResizeCallback( GLFWwindow *window, int width, int height )
{
    WindowWidth = width;
    WindowHeight = height;
    glViewport( 0, 0, width, height );
    shader->SetShaderValue( "Perspective", glm::perspectiveFov( (float)glm::radians( 90.f ), (float)width, (float)height, 0.0001f, 1000.f ) );
    glfwPollEvents();
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