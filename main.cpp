#include <cstdlib>
#include <iostream>

#include "shaders/shader.h"
#include "mesh/mesh.h"
#include "textures/texture.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>

static void KeyCallback( GLFWwindow *window, int key, int scancode, int action, int mods );
static void ResizeCallback( GLFWwindow *window, int width, int height );

long KeyFlags[ 6 ] { 0 };
static void SetKeyFlag( int key, bool set );
static bool GetKeyFlag( int key );

int main( int argc, const char *argv[] )
{
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

    if ( !gladLoadGLLoader( (GLADloadproc)glfwGetProcAddress ) )
        printf( "Couldn't initialize glad\n" );

    int Major, Minor;
    glGetIntegerv( GL_MAJOR_VERSION, &Major );
    glGetIntegerv( GL_MINOR_VERSION, &Minor );
    printf( "OpenGL version: %i.%i\n", Major, Minor );
    if ( Major < 3 || ( Major == 3 && Minor < 3 ) )
        printf( "OpenGL version not supported. Errors likely. Please update to 3.3\n" );

    int width, height;
    glfwGetFramebufferSize( window, &width, &height );
    glViewport( 0, 0, width, height );

    glfwSwapInterval( 1 );

    glEnable( GL_DEPTH_TEST );

    unsigned int shader = CreateShaderProgram();
    SetShaderValue( shader, "Transform", glm::translate( glm::mat4( 1 ), glm::vec3( 0, 0, 4 ) ) );
    SetShaderValue( shader, "Perspective", glm::ortho( -1, 1, -1, 1 ) );
    SetShaderValue( shader, "CameraTransform", glm::mat4( 1 ) );
    unsigned int texture = CreateTexture( "./textures/source/universe.png" );

    float verts[] = {
        0.5f, 0.5f, .0f,        1.0f, 1.0f,
        0.5f, -.5f, .0f,        1.0f, 0.0f,
        -.5f, -.5f, .0f,        0.0f, 0.0f,
        -.5f, 0.5f, .0f,        0.0f, 1.0f,
    };
    unsigned int inds[] = {
        0, 1, 3,
        1, 2, 3
    };
    Mesh *m = new Mesh( verts, sizeof( verts ) / sizeof( float ), inds, 6, shader, texture );

    double t = glfwGetTime();
    while ( !glfwWindowShouldClose( window ) )
    {
        double t_new = glfwGetTime();
        double dt = t_new - t;
        t = t_new;

        glClearColor( .2f, .3f, .3f, 1.0f );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        m->Render();

        glfwSwapBuffers( window );
        glfwPollEvents();
    }

    glfwDestroyWindow( window );

    delete m;
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
    glViewport( 0, 0, width, height );
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