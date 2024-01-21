#ifndef PHYSICS_H
#define PHYSICS_H

#pragma once

#include <glm/glm.hpp>
using glm::vec3;
using glm::mat3;

struct Element;

struct PhysicsObject
{
    PhysicsObject( Element &Object, float mass );

    void AddForce( vec3 F );
    void AddTorque( vec3 T );
    void AddOffCentreForce( vec3 F, vec3 pt );

    void ZeroMomentumIntoPlane( vec3 norm );

    void FrameUpdate();


    float mass;
    mat3 inertia_inv;

protected:

    Element &Object;

    vec3 _linear_momentum = vec3( 0 );
    vec3 _angular_momentum = vec3( 0 );
};

#endif
