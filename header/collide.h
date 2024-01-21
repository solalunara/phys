#ifndef COLLIDE_H
#define COLLIDE_H

#pragma once

#include <glm/glm.hpp>
#include <vector>

using std::vector;
using glm::vec3;

struct Element;
struct AABB;
struct Window;

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

struct Collide
{
    Collide( Element &object, vec3 mins, vec3 maxs );
    ~Collide();

    IntersectionData GetIntersection( Collide *other );
    void ResolveIntersection( Collide *other, IntersectionData data );

    Element    &object;
    const AABB *BoundingBox;

protected:
    Collide( Element &object, AABB *BoundingBox );

    AABB       *_BoundingBox;
};

struct AABB :
    public Collide
{
    AABB( Element &object, vec3 mins, vec3 maxs );

    bool TestAABBCollision( const AABB *other ) const;
    bool TestAABBCollision( vec3 other_mins, vec3 other_maxs ) const;

    vec3 mins;
    vec3 maxs;

private:
    vec3 __world_space_mins;
    vec3 __world_space_maxs;
};

vector<Collide> UTIL_TraceLine( Window *window, vec3 startpt, vec3 endpt );


#endif
