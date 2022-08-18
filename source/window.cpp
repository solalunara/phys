#include "window.h"
#include "mesh.h"
#include "entity.h"
#include "texture.h"

#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

using glm::zero;
using glm::identity;
using glm::one;
using glm::vec3;
using glm::quat;

Window::Window( WindowState state, int xres, int yres, const char *name ) :
    shader( NULL ), CameraTransform( zero<vec3>(), identity<quat>(), one<vec3>() )
{
    glfwDefaultWindowHints();
    const GLFWvidmode* mode = glfwGetVideoMode( glfwGetPrimaryMonitor() );
    switch ( state )
    {
        case WindowState::Windowed:
        ID = glfwCreateWindow( xres, yres, name, NULL, NULL );
        break;

        case WindowState::Borderless:
        glfwWindowHint( GLFW_RED_BITS, mode->redBits );
        glfwWindowHint( GLFW_GREEN_BITS, mode->greenBits );
        glfwWindowHint( GLFW_BLUE_BITS, mode->blueBits );
        glfwWindowHint( GLFW_REFRESH_RATE, mode->refreshRate );
        glfwWindowHint( GLFW_AUTO_ICONIFY, GLFW_FALSE );
        ID = glfwCreateWindow( xres, yres, name, glfwGetPrimaryMonitor(), NULL );
        break;

        case WindowState::Fullscreen:
        glfwWindowHint( GLFW_RED_BITS, mode->redBits );
        glfwWindowHint( GLFW_GREEN_BITS, mode->greenBits );
        glfwWindowHint( GLFW_BLUE_BITS, mode->blueBits );
        glfwWindowHint( GLFW_REFRESH_RATE, mode->refreshRate );
        ID = glfwCreateWindow( xres, yres, name, glfwGetPrimaryMonitor(), NULL );
        break;
    }

    if ( !ID )
    {
        const char *msg[ 1 ] { new char[ 1024 ] { 0 } };
        glfwGetError( msg );
        printf( "Couldn't create window - %s\n", msg[ 0 ] );
        printf( "Possible restart needed after driver update\n" );
    }

    glfwMakeContextCurrent( ID );
    glfwSetFramebufferSizeCallback( ID, ResizeCallback );
    if ( !Windows.size() )
    {
        MainWindow = this;
        if ( !gladLoadGLLoader( (GLADloadproc)glfwGetProcAddress ) )
            printf( "Couldn't initialize glad\n" );
    }
    shader = std::move( Shader() );

    Windows.push_back( this );

    int xsiz, ysiz;
    glfwGetFramebufferSize( MainWindow->ID, &xsiz, &ysiz );
    ResizeCallback( MainWindow->ID, xsiz, ysiz );

    glfwSwapInterval( 1 );
}

Window::~Window()
{
    glfwDestroyWindow( ID );
    for ( int i = 0; i < Meshes.size(); ++i )
        delete Meshes[ i ];
    for ( int i = 0; i < Entities.size(); ++i )
        delete Entities[ i ];
    for ( int i = 0; i < Textures.size(); ++i )
        delete Textures[ i ];
}

void Window::SetState( WindowState state, int xres, int yres )
{
    if ( state == CurrentState )
        return;

    //window hints don't actually do anything unless creating new window
    const GLFWvidmode* mode = glfwGetVideoMode( glfwGetPrimaryMonitor() );
    switch ( state )
    {
        case WindowState::Windowed:
        glfwSetWindowMonitor( ID, NULL, 0, 0, xres, yres, GLFW_DONT_CARE );
        break;

        case WindowState::Borderless:
        //glfwWindowHint( GLFW_RED_BITS, mode->redBits );
        //glfwWindowHint( GLFW_GREEN_BITS, mode->greenBits );
        //glfwWindowHint( GLFW_BLUE_BITS, mode->blueBits );
        glfwSetWindowAttrib( ID, GLFW_AUTO_ICONIFY, GLFW_FALSE );
        glfwSetWindowMonitor( ID, glfwGetPrimaryMonitor(), 0, 0, xres, yres, mode->refreshRate );
        break;

        case WindowState::Fullscreen:
        //glfwWindowHint( GLFW_RED_BITS, mode->redBits );
        //glfwWindowHint( GLFW_GREEN_BITS, mode->greenBits );
        //glfwWindowHint( GLFW_BLUE_BITS, mode->blueBits );
        glfwSetWindowAttrib( ID, GLFW_AUTO_ICONIFY, GLFW_TRUE );
        glfwSetWindowMonitor( ID, glfwGetPrimaryMonitor(), 0, 0, xres, yres, mode->refreshRate );
        break;
    }
}
void Window::Render()
{
    shader.SetShaderValue( "CameraTransform", CameraTransform.GetInverseMatrix() );
    for ( int i = 0; i < Meshes.size(); ++i )
        Meshes[ i ]->Render( NULL );
    for ( int i = 0; i < Entities.size(); ++i )
        Entities[ i ]->Render();
}

void ResizeCallback( GLFWwindow *window, int width, int height )
{
    glViewport( 0, 0, width, height );
    GetWindowFromID( window )->shader.SetShaderValue( "Perspective", glm::perspectiveFov( (float)glm::radians( 90.f ), (float)width, (float)height, 0.0001f, 1000.f ) );
    glfwPollEvents();
}