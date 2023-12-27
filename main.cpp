#include <cstdlib>
#include <iostream>
#include <vector> 
#include <map>
#include <filesystem>

using std::vector;
using std::map;
using std::filesystem::recursive_directory_iterator;

#include "shader.h"
#include "mesh.h"
#include "texture.h"
#include "cube.h"
#include "transform.h"
#include "window.h"
#include "GlobalTexture.h"
#include "text.h"
#include "axis.h"
#include "function.h"
#include "physics.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>

enum class Setting : char
{
    NONE = 0,
    KeyRotation = 1 << 0,
};

enum class InbuiltTexture : char
{
    NONE = 0,
    black = 1 << 0,
    dirt = 1 << 1,
    grass = 1 << 2,
    metal = 1 << 3,
    universe = 1 << 4,
};

map<Setting, int> Settings = {
    { Setting::KeyRotation, 0 }
};


void CheckOpenGLVersion()
{
    int Major, Minor;
    glGetIntegerv( GL_MAJOR_VERSION, &Major );
    glGetIntegerv( GL_MINOR_VERSION, &Minor );
    printf( "OpenGL version: %i.%i\n", Major, Minor );
    if ( Major < 3 || ( Major == 3 && Minor < 3 ) )
        printf( "OpenGL version not supported. Errors likely. Please update to 3.3\n" );
}

int WindowPreFrame( vector<Window *> Windows, int i )
{
    if ( glfwWindowShouldClose( Windows[ i ]->ID ) )
    {
        delete Windows[ i ];
        Windows.erase( Windows.begin() + i );
        if ( Windows.size() )
        {
            printf( "Window %d closed with exit code 0\n", i );
            return 1;
        }
        else
        {
            printf( "Process closed with exit code 0\n" );
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
    return 0;
}

void WindowRender( Window *window )
{
    if ( window->GetKeyFlag( GLFW_KEY_P ) )
    {
        window->SetKeyFlag( GLFW_KEY_P, false );
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

    window->Render();
}

void WindowPostFrame( Window *window )
{
    glfwSwapBuffers( window->ID );
    glfwPollEvents();
}

int CameraMovement( Window *window, float dt, map<Setting, int> Settings )
{
    if ( window->GetKeyFlag( GLFW_KEY_W ) )
        window->CameraTransform.pos += window->CameraTransform.LocalToWorldDirection( glm::vec3( 0, 0, -dt * 3 ) );
    if ( window->GetKeyFlag( GLFW_KEY_S ) )
        window->CameraTransform.pos += window->CameraTransform.LocalToWorldDirection( glm::vec3( 0, 0, +dt * 3 ) );
    if ( window->GetKeyFlag( GLFW_KEY_A ) )
        window->CameraTransform.pos += window->CameraTransform.LocalToWorldDirection( glm::vec3( -dt * 3, 0, 0 ) );
    if ( window->GetKeyFlag( GLFW_KEY_D ) )
        window->CameraTransform.pos += window->CameraTransform.LocalToWorldDirection( glm::vec3( +dt * 3, 0, 0 ) );
    if ( window->GetKeyFlag( GLFW_KEY_Q ) )
        window->CameraTransform.pos += window->CameraTransform.LocalToWorldDirection( glm::vec3( 0, +dt * 3, 0 ) );
    if ( window->GetKeyFlag( GLFW_KEY_E ) )
        window->CameraTransform.pos += window->CameraTransform.LocalToWorldDirection( glm::vec3( 0, -dt * 3, 0 ) );


    //can't do *= because it does the * in the wrong order (rot * new instaed of new * rot)
    if ( window->LockCursor )
    {
        if ( Settings[ Setting::KeyRotation ] ) //legacy key rotation as opposed to cursor rotation
        {
            if ( window->GetKeyFlag( GLFW_KEY_UP ) )
                window->CameraTransform.rot = glm::angleAxis( (float)glm::radians( +dt * 90 ), window->CameraTransform.LocalToWorldDirection( glm::vec3( 1, 0, 0 ) ) ) * window->CameraTransform.rot;
            if ( window->GetKeyFlag( GLFW_KEY_DOWN ) )
                window->CameraTransform.rot = glm::angleAxis( (float)glm::radians( -dt * 90 ), window->CameraTransform.LocalToWorldDirection( glm::vec3( 1, 0, 0 ) ) ) * window->CameraTransform.rot;
            if ( window->GetKeyFlag( GLFW_KEY_LEFT ) )
                window->CameraTransform.rot = glm::angleAxis( (float)glm::radians( +dt * 90 ), glm::vec3( 0, 1, 0 ) ) * window->CameraTransform.rot;
            if ( window->GetKeyFlag( GLFW_KEY_RIGHT ) )
                window->CameraTransform.rot = glm::angleAxis( (float)glm::radians( -dt * 90 ), glm::vec3( 0, 1, 0 ) ) * window->CameraTransform.rot;
        }
        else 
        {
            double xpos, ypos;
            int xsiz, ysiz;
            glfwGetCursorPos( window->ID, &xpos, &ypos );
            glfwGetFramebufferSize( window->ID, &xsiz, &ysiz );
            glfwSetCursorPos( window->ID, xsiz / 2.f, ysiz / 2.f );
            double xmv, ymv;
            xmv = xpos - xsiz / 2;
            ymv = ypos - ysiz / 2;
            window->CameraTransform.rot = glm::angleAxis( (float)glm::radians( -dt * 10 * ymv ), window->CameraTransform.LocalToWorldDirection( glm::vec3( 1, 0, 0 ) ) ) * window->CameraTransform.rot;
            window->CameraTransform.rot = glm::angleAxis( (float)glm::radians( -dt * 10 * xmv ), glm::vec3( 0, 1, 0 ) ) * window->CameraTransform.rot;
        }
    }

    return 0;
}

int main( int argc, const char *argv[] )
{
    if ( !glfwInit() )
        printf( "Couldn't initialize glfw\n" );

    // first window creation neccesary to init glad (gl____() functions)
    //          gl____() functions can't be initiated until a glfw context exists,
    //          but are also neccesary for certain operations on a window once created...
    //          so glad init is kind of stuck in the window constructor for now
    Window *main = new Window( WindowState::Windowed, 90.f, 1000, 768, "phys" );

    CheckOpenGLVersion();

    map<InbuiltTexture, GlobalTexture *> Textures = {
        { InbuiltTexture::black, new GlobalTexture( "./assets/textures/black.png" ) },
        { InbuiltTexture::dirt, new GlobalTexture( "./assets/textures/dirt.png" ) },
        { InbuiltTexture::grass, new GlobalTexture( "./assets/textures/grass.png" ) },
        { InbuiltTexture::metal, new GlobalTexture( "./assets/textures/metal.png" ) },
        { InbuiltTexture::universe, new GlobalTexture( "./assets/textures/universe.png" ) },
    };

    // for now just treat executable arguments as commands to make new windows
    for ( int i = 1; i < argc; ++i )
    {
        Window *w = new Window( WindowState::Windowed, 90.f, 200, 152, argv[ i ] );
        new Cube( glm::vec3( -.5f, -.5f, -.5f ), glm::vec3( .5f, .5f, .5f ), new Transform( glm::vec3( 0 ), glm::identity<glm::quat>(), glm::vec3( 1 ) ), Textures[ InbuiltTexture::universe ]->FindLocalTexture( w ), w );
    }

    //UIText *hello = new UIText( "helloo", 0, 0, 1.f, glm::vec3( 1.0f, 1.0f, 1.0f ), main );

    double t = glfwGetTime();
    double dt;

    GlobalTexture *GroundTextures[] = { 
        Textures[ InbuiltTexture::dirt ], 
        Textures[ InbuiltTexture::dirt ], 
        Textures[ InbuiltTexture::dirt ], 
        Textures[ InbuiltTexture::dirt ], 
        Textures[ InbuiltTexture::grass ], 
        Textures[ InbuiltTexture::dirt ] };

    //set the ground
    for ( int i = -10; i < 10; ++i )
        for ( int j = -10; j < 10; ++j )
            new Cube( glm::vec3( -.5f, -.5f, -.5f ), glm::vec3( .5f, .5f, .5f ), new Transform( glm::vec3( i, -3.f, j ), glm::identity<quat>(), glm::one<vec3>() ), 
            GroundTextures, main );
    
    Cube *PhysCube = new Cube( vec3( -.5f ), vec3( .5f ), new Transform( vec3( 0, 5, 0 ), glm::identity<quat>(), glm::one<vec3>() ), Textures[ InbuiltTexture::universe ], main );
    PhysCube->phys_obj = new PhysicsObject( *PhysCube, 1 );


    // fun with differential equations
    /*
    #define MGT 30.f
    glm::quat i = glm::angleAxis( glm::radians( 90.f ), vec3( 1, 0, 0 ) );
    Axis *x = new Axis( i_hat, vec3( 0, 0, 0 ), black, 10, 1, main );
    Axis *y = new Axis( j_hat, vec3( 0, 0, 0 ), black, 10, 1, main );
    Axis *z = new Axis( k_hat, vec3( 0, 0, 0 ), black, 10, 1, main );
    float wavelength = 2.f;
    float frequency = 1.f;
    float k = 2.f * M_PI / wavelength;
    float w = frequency * ( 2.f * M_PI );
    Transform *DefTransform = new Transform( glm::zero<vec3>(), glm::identity<quat>(), glm::one<vec3>() );
    DifferentialFunction *quantum = new DifferentialFunction( -10, 10, [] ( float x ) { return vec3( x, sin( 10 * x ) * exp( -( x * x ) ), cos( 10 * x ) * exp( -( x * x ) ) ); }, [ &quantum, i ] ( float x, float dx, float t, float dt ) {
        if ( x < quantum->min || x > quantum->max ) return vec3( x, 0, 0 );
        if ( dt <= 0 ) return quantum->PreviousState( x );
        vec3 prev = quantum->PreviousState( x );
        vec3 d2xdx2 = ( quantum->PreviousState( x + MGT * dx ) + quantum->PreviousState( x - MGT * dx ) - 2.f * prev ) / ( MGT * dx * MGT * dx );
        d2xdx2.x = 0;
        return -( i * d2xdx2 ) * dt / 100.f + prev;
    }, DefTransform, black, main, 0.01f );
    */

    main->CameraTransform.pos = vec3( 1, 0, 5 );

    while ( true )
    {
        //per frame for all windows
        double t_new = glfwGetTime();
        dt = t_new - t;
        t = t_new;
        DynamicFunction::FunctionTime = t;
        DifferentialFunction::FunctionDeltaTime = dt;

        //per frame per window
        for ( int i = 0; i < Windows.size(); ++i )
        {
            if ( WindowPreFrame( Windows, i ) )
            {
                --i;
                continue;
            }
            CameraMovement( Windows[ i ], dt, Settings );
            WindowRender( Windows[ i ] );
            WindowPostFrame( Windows[ i ] );
        }
    }
}
