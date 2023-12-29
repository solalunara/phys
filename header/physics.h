#ifndef PHYSICS_H
#define PHYSICS_H

#pragma once

#include <glm/glm.hpp>
using glm::vec3;

struct Element;

struct PhysicsObject
{
    PhysicsObject( Element &Object, float mass ) :
        Object( Object ), mass( mass )
    {}

    void AddImpulse( vec3 I );
    void ZeroMomentumAlongVector( vec3 v );

    void FrameUpdate( float dt );


    float mass;
protected:

    Element &Object;

    vec3 _linear_momentum = vec3( 0 );
    vec3 _angular_momentum = vec3( 0 );
};

#endif
