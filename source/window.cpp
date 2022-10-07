#include "window.h"
#include "mesh.h"
#include "cube.h"
#include "texture.h"
#include "GlobalTexture.h"
#include "element.h"

#include <string>
#include <string.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define NEARCLIP 0.0001f
#define FARCLIP 100000.f

using glm::zero;
using glm::identity;
using glm::one;
using glm::vec3;
using glm::quat;

Window::Window( WindowState state, float FOV, int xres, int yres, const char *name ) :
    shader( -1 ), 
    CameraTransform( zero<vec3>(), identity<quat>(), one<vec3>() ),
    UICameraTransform( zero<vec3>(), identity<quat>(), one<vec3>() ),
    Perspective( glm::perspectiveFov( (float)glm::radians( FOV ), (float)xres, (float)yres, NEARCLIP, FARCLIP ) ),
    UIPerspective( glm::ortho( -xres / 2.f, xres / 2.f, -yres / 2.f, yres / 2.f, NEARCLIP, FARCLIP ) ),
    FOV( FOV )
{
    this->name = new char[ strlen( name ) + 1 ];
    strcpy( this->name, name );
    glfwDefaultWindowHints();
    const GLFWvidmode* mode = glfwGetVideoMode( glfwGetPrimaryMonitor() );
    switch ( state )
    {
        case WindowState::Windowed:
        _ID = glfwCreateWindow( xres, yres, name, NULL, NULL );
        break;

        case WindowState::Borderless:
        glfwWindowHint( GLFW_RED_BITS, mode->redBits );
        glfwWindowHint( GLFW_GREEN_BITS, mode->greenBits );
        glfwWindowHint( GLFW_BLUE_BITS, mode->blueBits );
        glfwWindowHint( GLFW_REFRESH_RATE, mode->refreshRate );
        glfwWindowHint( GLFW_AUTO_ICONIFY, GLFW_FALSE );
        _ID = glfwCreateWindow( xres, yres, name, glfwGetPrimaryMonitor(), NULL );
        break;

        case WindowState::Fullscreen:
        glfwWindowHint( GLFW_RED_BITS, mode->redBits );
        glfwWindowHint( GLFW_GREEN_BITS, mode->greenBits );
        glfwWindowHint( GLFW_BLUE_BITS, mode->blueBits );
        glfwWindowHint( GLFW_REFRESH_RATE, mode->refreshRate );
        _ID = glfwCreateWindow( xres, yres, name, glfwGetPrimaryMonitor(), NULL );
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
    glfwSetKeyCallback( ID, KeyCallback );
    if ( !Windows.size() )
    {
        if ( !gladLoadGLLoader( (GLADloadproc)glfwGetProcAddress ) )
            printf( "Couldn't initialize glad\n" );
    }
    shader = std::move( Shader() );

    Windows.push_back( this );

    glfwSwapInterval( 1 );

    ResizeCallback( ID, xres, yres );

    glEnable( GL_DEPTH_TEST );
	glEnable( GL_FRAMEBUFFER_SRGB );
	//glEnable( GL_CULL_FACE );
	glDepthFunc( GL_LESS );
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );  

    //create local textures for every global texture instantiated
    for ( int i = 0; i < GlobalTextures.size(); ++i )
        new Texture( GlobalTextures[ i ]->path, this );
}

Window::~Window()
{
    delete name;
    glfwMakeContextCurrent( ID );
    glfwDestroyWindow( ID );
    for ( int i = 0; i < Elements.size(); ++i )
        delete Elements[ i ];
    for ( int i = 0; i < Textures.size(); ++i )
        delete Textures[ i ];
    for ( int i = Windows.size(); --i >= 0; ) //reverse b/c erasing elements of vector
    {
        if ( Windows[ i ] == this ) 
            Windows.erase( Windows.begin() + i );
        else
        {
            //refresh the viewport on other windows
            int xres, yres;
            glfwGetFramebufferSize( Windows[ i ]->ID, &xres, &yres );
            ResizeCallback( Windows[ i ]->ID, xres, yres );
            Windows[ i ]->Render();
        }
    }
    //make sure we delete the right shader
    glfwMakeContextCurrent( ID );
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
    shader.SetShaderValue( "UICameraTransform", UICameraTransform.GetInverseMatrix() );
    shader.SetShaderValue( "Perspective", Perspective );
    shader.SetShaderValue( "UIPerspective", UIPerspective );
    for ( int i = 0; i < Elements.size(); ++i )
        Elements[ i ]->Render();

    //int x, y;
    //glfwGetFramebufferSize( ID, &x, &y );
    //ResizeCallback( ID, x, y );
}

void Window::SetKeyFlag( int key, bool set )
{
    if ( set )
        KeyFlags[ key / 64 ] |= 1 << ( key % 64 );
    else
        KeyFlags[ key / 64 ] &= ~( 1 << ( key % 64 ) );
}
bool Window::GetKeyFlag( int key )
{
    return KeyFlags[ key / 64 ] & ( 1 << ( key % 64 ) );
}

void ResizeCallback( GLFWwindow *window, int width, int height )
{
    glfwMakeContextCurrent( window );
    glViewport( 0, 0, width, height );
    Window *w = Window::GetWindowFromID( window );
    w->Perspective = glm::perspectiveFov( (float)glm::radians( w->FOV ), (float)width, (float)height, NEARCLIP, FARCLIP );
    w->UIPerspective = glm::ortho( -width / 2.f, width / 2.f, -height / 2.f, height / 2.f, NEARCLIP, FARCLIP );
}

void KeyCallback( GLFWwindow *window, int key, int scancode, int action, int mods )
{
    switch ( action )
    {
        case GLFW_PRESS:
        Window::GetWindowFromID( window )->SetKeyFlag( key, true );
        break;

        case GLFW_RELEASE:
        Window::GetWindowFromID( window )->SetKeyFlag( key, false );
        break;
    }
}