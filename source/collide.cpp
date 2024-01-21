#include "collide.h"
#include "element.h"
#include "physics.h"
#include "window.h"
#include <cmath>
#include <stdexcept>

Collide::Collide( Element &object, AABB *BoundingBox ) :
    object( object ), BoundingBox( BoundingBox )
{}

Collide::Collide( Element &object, vec3 mins, vec3 maxs ) :
    object( object ), BoundingBox( new AABB( object, mins, maxs ) )
{}

Collide::~Collide()
{
    if ( (void *)BoundingBox != (void *)this )
        delete BoundingBox;
}

IntersectionData Collide::GetIntersection( Collide *other )
{
    //don't do self collisions
    if ( other == this )
        throw std::invalid_argument( "Collide::GetIntersection called on same object - unoptimized code likely" );

    //only test for collision if BBoxes are colliding
    if ( !BoundingBox->TestAABBCollision( other->BoundingBox ) )
        return IntersectionData( 0, glm::zero<vec3>(), false );

    //we can only resolve collisions if there is at least one physics object - don't bother with static objects    
    if ( !object.phys_obj && !other->object.phys_obj )
        return IntersectionData( 0, glm::zero<vec3>(), false );


    // project the points from each object onto each normal vector for both objects
    // if there is no collision then at least one of the penetration values must be zero
    // if there is no zero value along any of the normals then there is a collision with a penetration equal to the lowest penetration normal
    // this function will calculate the normal of the collision as well as the penetration value

    vector<vec3> norms1 = object.GetNormals();
    vector<vec3> norms2 = other->object.GetNormals();
    vector<vec3> norms;
    norms.reserve( norms1.size() + norms2.size() );
    for ( int i = 0; i < norms1.size(); ++i )
        norms.push_back( norms1[ i ] );
    for ( int i = 0; i < norms2.size(); ++i )
        norms.push_back( norms2[ i ] );

    vector<vec3> otherpts = other->object.GetVertices();

    return GetIntersection( norms, otherpts );
}
IntersectionData Collide::GetIntersection( vector<vec3> norms, vector<vec3> pts )
{
    vector<float> penetration_vals;
    penetration_vals.reserve( norms.size() );

    vector<vec3> Points1 = object.GetVertices();
    vector<vec3> Points2 = pts;
    if ( Points1.size() == 0 || Points2.size() == 0 )
        return IntersectionData( 0, glm::zero<vec3>(), false );

    float *ProjectedPoints1 = new float[ Points1.size() ];
    float *ProjectedPoints2 = new float[ Points2.size() ];
    for ( int n = 0; n < norms.size(); ++n )
    {
        vec3 Normal = norms[ n ];


        for ( int i = 0; i < Points1.size(); ++i )
            ProjectedPoints1[ i ] = glm::dot( Points1[ i ], Normal );
        for ( int i = 0; i < Points2.size(); ++i )
            ProjectedPoints2[ i ] = glm::dot( Points2[ i ], Normal );

        float amin, amax, bmin, bmax;
        amax = bmax = -INFINITY;
        amin = bmin = INFINITY;
        for ( int i = 0; i < Points1.size(); ++i )
        {
            if ( ProjectedPoints1[ i ] < amin )
                amin = ProjectedPoints1[ i ];
            if ( ProjectedPoints1[ i ] > amax )
                amax = ProjectedPoints1[ i ];
        }
        for ( int i = 0; i < Points2.size(); ++i )
        {
            if ( ProjectedPoints2[ i ] < bmin )
                bmin = ProjectedPoints2[ i ];
            if ( ProjectedPoints2[ i ] > bmax )
                bmax = ProjectedPoints2[ i ];
        }

        if ( amin > bmin && amax < bmax )
            penetration_vals.push_back( abs( amin - bmin ) < abs( amax - bmax ) ? bmin - amax : bmax - amin ); //a enclosed in b
        else if ( bmin > amin && bmax < amax )
            penetration_vals.push_back( abs( amin - bmin ) < abs( amax - bmax ) ? amin - bmax : amax - bmin ); //b enclosed in a
        else if ( amin <= bmax && amin >= bmin )
            penetration_vals.push_back( bmax - amin ); // a > b, thus positive for pushing a positive
        else if ( bmin <= amax && bmin >= amin )
            penetration_vals.push_back( -( amax - bmin ) ); // a < b, thus negative for pushing a negative
        else
            penetration_vals.push_back( 0 );
    }
    delete[] ProjectedPoints1;
    delete[] ProjectedPoints2;

    // if there exists an axis with 0 penetration, we are not colliding
    // otherwise, we want the penetration with the lowest absolute magnitude
    float MinPen = INFINITY;
    int MinPen_index = -1;
    for ( int i = 0; i < penetration_vals.size(); ++i )
    {
        if ( std::abs( penetration_vals[ i ] ) < MinPen ||
            ( std::abs( penetration_vals[ i ] ) <= MinPen && penetration_vals[ i ] > 0 ) )
        {
            MinPen = std::abs( penetration_vals[ i ] );
            MinPen_index = i;
        }
    }
    if ( MinPen == 0 )
        return IntersectionData( 0, glm::zero<vec3>(), false );
    else return IntersectionData( penetration_vals[ MinPen_index ], norms[ MinPen_index ], true );
}
IntersectionData Collide::GetIntersection( vec3 mins, vec3 maxs )
{
    vector<vec3> pts;
    pts.reserve( 8 );

    pts.push_back( vec3( mins.x, mins.y, mins.z ) );
    pts.push_back( vec3( mins.x, maxs.y, mins.z ) );
    pts.push_back( vec3( mins.x, mins.y, maxs.z ) );
    pts.push_back( vec3( mins.x, maxs.y, maxs.z ) );

    pts.push_back( vec3( maxs.x, mins.y, mins.z ) );
    pts.push_back( vec3( maxs.x, maxs.y, mins.z ) );
    pts.push_back( vec3( maxs.x, mins.y, maxs.z ) );
    pts.push_back( vec3( maxs.x, maxs.y, maxs.z ) );


    vector<vec3> norms;
    norms.reserve( 6 );

    norms.push_back( vec3(  1,  0,  0 ) );
    norms.push_back( vec3( -1,  0,  0 ) );
    norms.push_back( vec3(  0,  1,  0 ) );
    norms.push_back( vec3(  0, -1,  0 ) );
    norms.push_back( vec3(  0,  0,  1 ) );
    norms.push_back( vec3(  0,  0, -1 ) );

    return GetIntersection( norms, pts );
}

void Collide::ResolveIntersection( Collide *other, IntersectionData data )
{
    if ( !object.phys_obj )
        throw std::invalid_argument( "Collide::ResolveIntersection called on static object (no physics object)!" );

    if ( !data.Intersection )
        throw std::invalid_argument( "Collide::ResolveIntersection called on non-intersection" );

    if ( other->object.phys_obj )
    {
        float MassFraction = object.phys_obj->mass / other->object.phys_obj->mass;

        object.transform->SetAbsOrigin( object.transform->GetAbsOrigin() + MassFraction * data.Penetration * data.Normal );
        other->object.transform->SetAbsOrigin( other->object.transform->GetAbsOrigin() + ( 1 / MassFraction ) * data.Penetration * data.Normal );
    }
    else
    {
        object.transform->SetAbsOrigin( object.transform->GetAbsOrigin() + data.Penetration * data.Normal );
        object.phys_obj->ZeroMomentumIntoPlane( data.Normal );
        object.phys_obj->AddOffCentreForce( data.Penetration * data.Normal, vec3( 0, 0, 1.f ) );
    }
}

AABB::AABB( Element &object, vec3 mins, vec3 maxs ) :
    Collide( object, this ), mins( mins ), maxs( maxs ), __world_space_mins( object.transform->LocalToWorldPoint( mins ) ), __world_space_maxs( object.transform->LocalToWorldPoint( maxs ) )
{}

bool AABB::TestAABBCollision( const AABB *other ) const
{
    // get world space boundaries
    // if we don't have a physics object, we're static, so use cached mins and maxs
    vec3 ThisWorldSpaceMins;
    vec3 ThisWorldSpaceMaxs;
    if ( !object.phys_obj )
    {
        ThisWorldSpaceMins = __world_space_mins;
        ThisWorldSpaceMaxs = __world_space_maxs;
    }
    else
    {
        ThisWorldSpaceMins = object.transform->GetAbsOrigin() + mins;
        ThisWorldSpaceMaxs = object.transform->GetAbsOrigin() + maxs;
    }

    vec3 OtherWorldSpaceMins;
    vec3 OtherWorldSpaceMaxs;
    if ( !other->object.phys_obj )
    {
        OtherWorldSpaceMins = other->__world_space_mins;
        OtherWorldSpaceMaxs = other->__world_space_maxs;
    }
    else
    {
        OtherWorldSpaceMins = other->object.transform->GetAbsOrigin() + other->mins;
        OtherWorldSpaceMaxs = other->object.transform->GetAbsOrigin() + other->maxs;
    }


    return  ThisWorldSpaceMins.x <= OtherWorldSpaceMaxs.x &&
            ThisWorldSpaceMaxs.x >= OtherWorldSpaceMins.x &&
            ThisWorldSpaceMins.y <= OtherWorldSpaceMaxs.y &&
            ThisWorldSpaceMaxs.y >= OtherWorldSpaceMins.y &&
            ThisWorldSpaceMins.z <= OtherWorldSpaceMaxs.z &&
            ThisWorldSpaceMaxs.z >= OtherWorldSpaceMins.z;
}
bool AABB::TestAABBCollision( vec3 other_mins, vec3 other_maxs ) const
{
    // get world space boundaries
    // if we don't have a physics object, we're static, so use cached mins and maxs
    vec3 ThisWorldSpaceMins;
    vec3 ThisWorldSpaceMaxs;
    if ( !object.phys_obj )
    {
        ThisWorldSpaceMins = __world_space_mins;
        ThisWorldSpaceMaxs = __world_space_maxs;
    }
    else
    {
        ThisWorldSpaceMins = object.transform->GetAbsOrigin() + mins;
        ThisWorldSpaceMaxs = object.transform->GetAbsOrigin() + maxs;
    }

    vec3 OtherWorldSpaceMins = other_mins;
    vec3 OtherWorldSpaceMaxs = other_maxs;

    return  ThisWorldSpaceMins.x <= OtherWorldSpaceMaxs.x &&
            ThisWorldSpaceMaxs.x >= OtherWorldSpaceMins.x &&
            ThisWorldSpaceMins.y <= OtherWorldSpaceMaxs.y &&
            ThisWorldSpaceMaxs.y >= OtherWorldSpaceMins.y &&
            ThisWorldSpaceMins.z <= OtherWorldSpaceMaxs.z &&
            ThisWorldSpaceMaxs.z >= OtherWorldSpaceMins.z;
}

vector<Collide> UTIL_TraceLine( Window *window, vec3 startpt, vec3 endpt )
{
    for ( int i = 0; i < window->Elements.size(); ++i )
    {
        if ( window->Elements[ i ]->collide && window->Elements[ i ]->collide->BoundingBox->TestAABBCollision( startpt, endpt ) )
        {

        }
    }
    return vector<Collide>();
}