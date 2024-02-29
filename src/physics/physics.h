#ifndef PHYSICS_H
#define PHYSICS_H

#pragma once

#include <glm/glm.hpp>
using glm::vec3;
using glm::mat3;

struct Element;

struct PhysicsBaseObject
{
    PhysicsBaseObject( float mass, vec3 mins, vec3 maxs );

    void AddForce( vec3 F );
    void AddTorque( vec3 T );

    void ZeroMomentumIntoPlane( vec3 norm );

    float mass;
    mat3 inertia_inv;

    const vec3 &linear_momentum = _linear_momentum;
    const vec3 &angular_momentum = _angular_momentum;

protected:
    PhysicsBaseObject( float mass );
    vec3 _linear_momentum = vec3( 0 );
    vec3 _angular_momentum = vec3( 0 );
};

struct PhysicsObject :
    public PhysicsBaseObject
{
    PhysicsObject( Element &Object, float mass );

    void AddOffCentreForce( vec3 F, vec3 pt );

    void FrameUpdate();

    float mass;
    mat3 inertia_inv;

protected:
    Element &Object;
};

#endif
