#include "physics.h"
#include "element.h"


void PhysicsObject::AddImpulse( vec3 I )
{
    _linear_momentum += I;
}

void PhysicsObject::FrameUpdate( float dt )
{
    AddImpulse( vec3( 0, -mass / 10, 0 ) * dt );
    Object.transform->pos += dt * _linear_momentum / mass;
}