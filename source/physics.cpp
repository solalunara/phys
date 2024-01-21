#include "physics.h"
#include "element.h"
#include "function.h"
#include "collide.h"

#define FIXED_FRAME_TIME ( 1 / 60.f )

PhysicsObject::PhysicsObject( Element &Object, float mass ) :
    Object( Object ), mass( mass )
{
    //for now, assume uniform density
    mat3 inertia;
    vector<vec3> vertices = Object.GetVertices( true );
    vec3 maxs = glm::zero<vec3>();
    vec3 mins = glm::zero<vec3>();
    for ( int i = 0; i < vertices.size(); ++i )
    {
        for ( int j = 0; j < 3; ++j )
        {
            if ( maxs[ j ] < vertices[ i ][ j ] )
                maxs[ j ] = vertices[ i ][ j ];
            if ( mins[ j ] > vertices[ i ][ j ] )
                mins[ j ] = vertices[ i ][ j ];
        }
    }
    vec3 size = maxs - mins;

    // I_ij = integral r^2 delta_ij - r_i r_j dm
    for ( int i = 0; i < 3; ++i )
    {
        for ( int j = 0; j < 3; ++j )
        {
            inertia[ i ][ j ] = 0.f;
            if ( i == j )
            {
                for ( int k = 0; k < 3; ++k )
                {
                    if ( k != i )
                        inertia[ i ][ j ] += powf( maxs[ k ], 3.f ) / 3.f;
                }
            }
            else
            {
                for ( int k = 0; k < 3; ++k )
                    inertia[ i ][ j ] -= maxs[ k ] * maxs[ k ] * mins[ k ] * mins[ k ] / 4.f;
            }
        }
    }
    float density = mass / ( size.x * size.y * size.z );
    inertia = density * inertia;

    inertia_inv = glm::inverse( inertia );
}

void PhysicsObject::AddForce( vec3 F )
{
#ifdef FIXED_FRAME_TIME
    _linear_momentum += F * FIXED_FRAME_TIME;
#else
    _linear_momentum += F * DifferentialFunction::FunctionDeltaTime;
#endif
}

void PhysicsObject::AddTorque( vec3 T )
{
#ifdef FIXED_FRAME_TIME
    _angular_momentum += T * FIXED_FRAME_TIME;
#else
    _angular_momentum += T * DifferentialFunction::FunctionDeltaTime;
#endif
}

void PhysicsObject::AddOffCentreForce( vec3 F, vec3 pt )
{
    AddTorque( glm::cross( pt, F ) );
}

void PhysicsObject::ZeroMomentumIntoPlane( vec3 norm )
{
    if ( glm::dot( _linear_momentum, norm ) < 0 )
        _linear_momentum -= norm * glm::dot( _linear_momentum, norm );
}

void PhysicsObject::FrameUpdate()
{
#ifdef FIXED_FRAME_TIME
    AddForce( vec3( 0, -mass, 0 ) );
    Object.transform->pos += FIXED_FRAME_TIME * _linear_momentum / mass;

    if ( _angular_momentum != glm::zero<vec3>() )
    {
        vec3 angular_velocity = inertia_inv * _angular_momentum;
        quat q_angular_velocity = glm::angleAxis( FIXED_FRAME_TIME * glm::length( angular_velocity ), glm::normalize( angular_velocity ) );
        Object.transform->rot = q_angular_velocity * Object.transform->rot;
    }
#else
    AddImpulse( vec3( 0, -mass, 0 ) * DifferentialFunction::FunctionDeltaTime );
    Object.transform->pos += DifferentialFunction::FunctionDeltaTime * _linear_momentum / mass;

    if ( _angular_momentum != glm::zero<vec3>() )
    {
        vec3 angular_velocity = inertia_inv * _angular_momentum;
        quat q_angular_velocity = glm::angleAxis( DifferentialFunction::FunctionDeltaTime * glm::length( angular_velocity ), glm::normalize( angular_velocity ) );
        Object.transform->rot = q_angular_velocity * Object.transform->rot;
    }
#endif
}