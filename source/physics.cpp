#include "physics.h"
#include "element.h"

#define FIXED_FRAME_TIME

void PhysicsObject::AddImpulse( vec3 I )
{
    _linear_momentum += I;
}

void PhysicsObject::ZeroMomentumAlongVector( vec3 v )
{
    if ( glm::dot( _linear_momentum, v ) > 0 )
        _linear_momentum -= v * glm::dot( _linear_momentum, v );
}

void PhysicsObject::FrameUpdate( float dt )
{
#ifdef FIXED_FRAME_TIME
    AddImpulse( vec3( 0, -mass, 0 ) / 60.f );
    Object.transform->pos += ( 1 / 60.f ) * _linear_momentum / mass;
#else
    AddImpulse( vec3( 0, -mass, 0 ) * dt );
    Object.transform->pos += dt * _linear_momentum / mass;
#endif
}