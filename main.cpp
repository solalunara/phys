#include <cstdlib>
#include <iostream>
using namespace std;

#include <glad/glad.h>
#include <GLFW/glfw3.h>

int main( int argc, const char *argv[] )
{
    if ( !glfwInit() )
        printf( "Couldn't initialize glfw\n" );

    GLFWwindow *window = glfwCreateWindow( 768, 480, "phys", NULL, NULL );
    if ( !window )
        printf( "Please update your version of OpenGL to 3.3 or later\n" );
    glfwMakeContextCurrent( window );

    if ( !gladLoadGLLoader( (GLADloadproc)glfwGetProcAddress ) )
        printf( "Couldn't initialize glad\n" );

    int Major, Minor;
    glGetIntegerv( GL_MAJOR_VERSION, &Major );
    glGetIntegerv( GL_MINOR_VERSION, &Minor );
    printf( "OpenGL version: %i.%i\n", Major, Minor );
    if ( Major < 3 || ( Major == 3 && Minor < 3 ) )
        printf( "OpenGL version not supported. Errors likely. Please update to 3.3\n" );

    while ( !glfwWindowShouldClose( window ) )
    {
        glfwPollEvents();
    }

    glfwTerminate();
}