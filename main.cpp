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
#include "collide.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

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

    if ( window->GetKeyFlag( GLFW_KEY_SPACE ) )
    {
        window->SetKeyFlag( GLFW_KEY_SPACE, false );

        DifferentialFunction::FunctionDeltaTime = 1 / 10.f;
        DynamicFunction::FunctionTime += 1 / 10.f;
    }


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
    #define INT_GROUND_SIZE 10
    for ( int i = -INT_GROUND_SIZE; i <= INT_GROUND_SIZE; ++i )
        for ( int j = -INT_GROUND_SIZE; j <= INT_GROUND_SIZE; ++j )
        {
            Cube *c = new Cube( glm::vec3( -.5f, -.5f, -.5f ), glm::vec3( .5f, .5f, .5f ), new Transform( glm::vec3( i, -3.f, j ), glm::identity<quat>(), glm::one<vec3>() ), GroundTextures, main );
            c->collide = new AABB( *c, glm::vec3( -.5f ), glm::vec3( .5f ) );
        }
    
    //Cube *PhysCube = new Cube( vec3( -.5f ), vec3( .5f ), new Transform( vec3( 0, 5, 0 ), glm::identity<quat>(), glm::one<vec3>() ), Textures[ InbuiltTexture::universe ], main );
    //PhysCube->transform->rot = glm::angleAxis( glm::radians( 45.f ), glm::vec3( 1, 0, 0 ) );
    //PhysCube->phys_obj = new PhysicsObject( *PhysCube, 1 );
    //PhysCube->collide = new Collide( *PhysCube, glm::vec3( -1.f ), glm::vec3( 1.f ) );

    //Cube *PhysCube2 = new Cube( vec3( -.5f ), vec3( .5f ), new Transform( vec3( 0, 10, 0 ), glm::identity<quat>(), glm::one<vec3>() ), Textures[ InbuiltTexture::universe ], main );
    //PhysCube2->transform->rot = glm::angleAxis( glm::radians( 30.f ), glm::vec3( 1, 0, 0 ) );
    //PhysCube2->phys_obj = new PhysicsObject( *PhysCube2, 1 );
    //PhysCube2->collide = new Collide( *PhysCube2, glm::vec3( -1.f ), glm::vec3( 1.f ) );


    // fun with differential equations
    
    glm::quat i = glm::angleAxis( glm::radians( 90.f ), vec3( 1, 0, 0 ) );
    Axis *x = new Axis( i_hat, vec3( 0, 0, 0 ), Textures[ InbuiltTexture::black ], 10, 1, main );
    Axis *y = new Axis( j_hat, vec3( 0, 0, 0 ), Textures[ InbuiltTexture::black ], 10, 1, main );
    Axis *z = new Axis( k_hat, vec3( 0, 0, 0 ), Textures[ InbuiltTexture::black ], 10, 1, main );
    float p = 1;
    float m = 1;
    float hbar = 1;
    Transform *DefTransform = new Transform( glm::zero<vec3>(), glm::identity<quat>(), glm::one<vec3>() );
    DifferentialFunction *quantum = new DifferentialFunction( -3, 3, [] ( float x ) { return vec2( exp( -( x * x ) ), 0 ); }, 
        // pf/pt = F[ f, pf/px, p2f/px2, x, t ]
        [ p, m, hbar ] ( vec2 f, vec2 pfpx, vec2 p2fpx2, float x, float t ) {
        vec2 val = p * p / 2 / m / hbar * p2fpx2 + x * x / hbar * f;
        return vec2( val.y, -val.x ) / 1000.f;
        //return vec2( 0, 0 );
    }, DefTransform, Textures[ InbuiltTexture::black ], main, 0.01f );

    DifferentialFunction *sinfunc = new DifferentialFunction( -10, 10, [] ( float x ) { return vec2( 0, sin( x ) ); },
    [] ( vec2 f, vec2 pfpx, vec2 p2fpx2, float x, float t ) {
        return vec2( 0, 0 );
    }, DefTransform, Textures[ InbuiltTexture::dirt ], main );
    sinfunc->GenPrevState();
    StaticFunction *cosfunc = sinfunc->FourierFnApprox( sinfunc->FourierDerivative( sinfunc->FourierApproximation( 100 ) ), -10, 10, Textures[ InbuiltTexture::universe ], main );

    main->CameraTransform.pos = vec3( 1, 0, 3 );

    vec3 player_mins = vec3( -.2f, -1.2f, -.2f );
    vec3 player_maxs = vec3(  .2f,   .2f,  .2f );
    //main->PlayerPhysics = new PhysicsBaseObject( 10.f, player_mins, player_maxs );

    while ( true )
    {
        //per frame for all windows

        double t_new = glfwGetTime();
        dt = t_new - t;
        t = t_new;
        //DynamicFunction::FunctionTime = t;
        //DifferentialFunction::FunctionDeltaTime = dt;

        //per frame per window
        for ( int i = 0; i < Windows.size(); ++i )
        {
            if ( WindowPreFrame( Windows, i ) )
            {
                --i;
                continue;
            }
            CameraMovement( Windows[ i ], dt, Settings );
            if ( Windows[ i ]->PlayerPhysics )
            {
                //player collision detection/resolution
                for ( int j = 0; j < Windows[ i ]->Elements.size(); ++j ) 
                {
                    if ( Windows[ i ]->Elements[ j ]->collide )
                    {
                        IntersectionData data = Windows[ i ]->Elements[ j ]->collide->GetIntersection( player_mins + Windows[ i ]->CameraTransform.pos, player_maxs + Windows[ i ]->CameraTransform.pos );
                        if ( data.Intersection )
                        {
                            data.Normal = -data.Normal; //switch to player frame

                            //player collision resolution code
                            Collide *other = Windows[ i ]->Elements[ j ]->collide;
                            if ( other->object.phys_obj )
                            {
                                float MassFraction = Windows[ i ]->PlayerPhysics->mass / other->object.phys_obj->mass;

                                //Windows[ i ]->CameraTransform.pos += MassFraction * data.Penetration * data.Normal;
                                //other->object.transform->SetAbsOrigin( other->object.transform->GetAbsOrigin() + ( 1 / MassFraction ) * data.Penetration * data.Normal );
                            }
                            else
                            {
                                //Windows[ i ]->CameraTransform.pos += data.Penetration * data.Normal;
                                Windows[ i ]->PlayerPhysics->ZeroMomentumIntoPlane( data.Normal );
                            }
                        }
                            
                    }
                }
                //player physics tick
                Windows[ i ]->PlayerPhysics->AddForce( vec3( 0, -Windows[ i ]->PlayerPhysics->mass, 0 ) );
                Windows[ i ]->CameraTransform.pos += ( 1 / 60.f ) * Windows[ i ]->PlayerPhysics->linear_momentum / Windows[ i ]->PlayerPhysics->mass;

                if ( Windows[ i ]->PlayerPhysics->angular_momentum != glm::zero<vec3>() )
                {
                    vec3 angular_velocity = Windows[ i ]->PlayerPhysics->inertia_inv * Windows[ i ]->PlayerPhysics->angular_momentum;
                    quat q_angular_velocity = glm::angleAxis( ( 1 / 60.f ) * glm::length( angular_velocity ), glm::normalize( angular_velocity ) );
                    Windows[ i ]->CameraTransform.rot = q_angular_velocity * Windows[ i ]->CameraTransform.rot;
                }
            }


            StaticFunction *approx_0 = NULL;
            StaticFunction *approx_1 = NULL;
            if ( quantum->PreviousStateSave )
            {
                //approx_0 = quantum->FourierFnApprox( quantum->FourierApproximation( 100 ), -5, 5, Textures[ InbuiltTexture::universe ], main );
                approx_1 = quantum->FourierFnApprox( quantum->FourierDerivative( quantum->FourierDerivative( quantum->FourierApproximation( 100 ) ) ), -5, 5, Textures[ InbuiltTexture::universe ], main );
            }

            WindowRender( Windows[ i ] );

            if ( approx_0 )
                delete approx_0;
            if ( approx_1 )
                delete approx_1;

            WindowPostFrame( Windows[ i ] );
        }
        if ( DifferentialFunction::FunctionDeltaTime != 0 )
            DifferentialFunction::FunctionDeltaTime = 0;
    }
}
