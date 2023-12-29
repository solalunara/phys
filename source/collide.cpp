#include "collide.h"
#include "element.h"
#include "physics.h"
#include <cmath>

IntersectionData Collide::GetIntersection( Collide *other )
{
    if ( other == this )
        return IntersectionData( 0, glm::zero<vec3>(), false );

    vector<vec3> norms1 = object.GetNormals();
    vector<vec3> norms2 = other->object.GetNormals();
    vector<vec3> norms;
    for ( int i = 0; i < norms1.size(); ++i )
        norms.push_back( norms1[ i ] );
    for ( int i = 0; i < norms2.size(); ++i )
        norms.push_back( norms2[ i ] );

    vector<float> penetration_vals;

    vector<vec3> Points1 = object.GetVertices();
    vector<vec3> Points2 = other->object.GetVertices();
    if ( Points1.size() == 0 || Points2.size() == 0 )
        return IntersectionData( 0, glm::zero<vec3>(), false );

    for ( int n = 0; n < norms.size(); ++n )
    {
        vec3 Normal = norms[ n ];

        float *ProjectedPoints1 = new float[ Points1.size() ];
        float *ProjectedPoints2 = new float[ Points2.size() ];

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
            penetration_vals.push_back( abs( amin - bmin ) < abs( amax - bmax ) ? bmin - amin : bmax - amax ); //a enclosed in b
        else if ( bmin > amin && bmax < amax )
            penetration_vals.push_back( abs( amin - bmin ) < abs( amax - bmax ) ? amin - bmin : amax - bmax ); //b enclosed in a
        else if ( amin <= bmax && amin >= bmin )
            penetration_vals.push_back( bmax - amin ); // a > b, thus positive for pushing a positive
        else if ( bmin <= amax && bmin >= amin )
            penetration_vals.push_back( -( amax - bmin ) ); // a < b, thus negative for pushing a negative
        else
            penetration_vals.push_back( 0 );
    }

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

void Collide::ResolveIntersection( Collide *other, IntersectionData data )
{
    if ( !data.Intersection || !object.phys_obj )
        return;

    if ( other->object.phys_obj )
    {
        float MassFraction = object.phys_obj->mass / other->object.phys_obj->mass;

        object.transform->SetAbsOrigin( object.transform->GetAbsOrigin() + MassFraction * data.Penetration * data.Normal );
        other->object.transform->SetAbsOrigin( other->object.transform->GetAbsOrigin() + ( 1 / MassFraction ) * data.Penetration * data.Normal );
    }
    else
    {
        object.transform->SetAbsOrigin( object.transform->GetAbsOrigin() + data.Penetration * data.Normal );
        object.phys_obj->ZeroMomentumAlongVector( data.Normal );
    }
}