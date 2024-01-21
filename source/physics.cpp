#include "physics.h"
#include "element.h"

#define FIXED_FRAME_TIME

void PhysicsObject::AddImpulse( vec3 I )
{
    _linear_momentum += I;
}

void PhysicsObject::ZeroMomentumIntoPlane( vec3 norm )
{
    if ( glm::dot( _linear_momentum, norm ) < 0 )
        _linear_momentum -= norm * glm::dot( _linear_momentum, norm );
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