#ifndef COLLIDE_H
#define COLLIDE_H

#pragma once

#include <glm/glm.hpp>
#include <vector>

using std::vector;
using glm::vec3;

struct Element;

enum class CollideType : char
{
    ConvexShape = 0,

    AxisAlignedBoundingBox = 1 << 0,
    AABB = AxisAlignedBoundingBox,

    OrientedBoundingBox = 1 << 1,
    OBB = OrientedBoundingBox,

    Sphere = 1 << 2,
    Ellipsoid = 1 << 3,
};

struct IntersectionData
{
    IntersectionData( float Penetration, vec3 Normal, bool Intersection ) :
        Penetration( Penetration ), Normal( Normal ), Intersection( Intersection )
    {}

    float Penetration;
    vec3 Normal;
    bool Intersection;
};

struct AABB;

struct Collide
{
    Collide( Element &object, vec3 mins, vec3 maxs );
    ~Collide();

    IntersectionData GetIntersection( Collide *other );
    void ResolveIntersection( Collide *other, IntersectionData data );

    Element         &object;

protected:
    Collide( Element &object, AABB *BoundingBox );

    AABB            *BoundingBox;
};

struct AABB :
    public Collide
{
    AABB( Element &object, vec3 mins, vec3 maxs ) :
        Collide( object, this ), mins( mins ), maxs( maxs )
    {}

    bool TestAABBCollision( AABB *other );

    vec3 mins;
    vec3 maxs;
};


#endif
