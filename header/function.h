#ifndef FUNCTION_H
#define FUNCTION_H

#pragma once

#include "cube.h"
#include <glm/vec3.hpp>
#include <vector>
#include <functional>
using glm::vec3;
using std::function;

struct StaticFunction :
    public GameElement
{
    //have to have explicit glm call b/c it thinks vec3 is a function otherwise
    StaticFunction( float min, float max, function<vec2( float )> fn, Transform *transform, GlobalTexture *tex, Window *w, float step = 0.03f ) :
        GameElement( w, transform )
    {
        u_long n_Segments = (u_long)( ( max - min ) / step ) + 1;
        for ( u_long i = 0; i < n_Segments; ++i )
        {
            AddElement( new Cube(
                vec3( -step / 2 ),
                vec3(  step / 2 ),
                new Transform( vec3( i*step+min, fn( i * step + min ) ), glm::identity<quat>(), glm::one<vec3>() ),
                tex->FindLocalTexture( w ),
                w
            ) );
        }
    }
};

struct DynamicFunction :
    public GameElement
{
    DynamicFunction( float min, float max, function<vec2( float, float )> fn, Transform *transform, GlobalTexture *tex, Window *w, float step = 0.03f ) :
        GameElement( w, transform ), fn( fn ), min( min ), step( step )
    {
        u_long n_Segments = (u_long)( ( max - min ) / step ) + 1;
        for ( u_long i = 0; i < n_Segments; ++i )
        {
            AddElement( new Cube(
                vec3( -step / 2 ),
                vec3(  step / 2 ),
                new Transform( vec3( min+step*i, fn( i * step + min, 0 ) ), glm::identity<quat>(), glm::one<vec3>() ),
                tex->FindLocalTexture( w ),
                w
            ) );
        }
    }

    virtual void Render()
    {
        for ( u_long i = 0; i < Elements.size(); ++i )
        {
            vec2 p = fn( i * step + min, FunctionTime );
            Elements[ i ]->transform->pos.x = p.x;
            Elements[ i ]->transform->pos.x = p.y;
        }
        Element::Render();
    }

    function<vec2( float, float )> fn;
    float min, step;

    static inline float FunctionTime;
};


// f = f[x, dx, t, dt]
struct DifferentialFunction :
    public GameElement
{
    DifferentialFunction( float min, float max, function<vec2( float )> InitialState, function<vec2( vec2, vec2, vec2, float, float )> fn, Transform *transform, GlobalTexture *tex, Window *w, float step = 0.03f ) :
        GameElement( w, transform ), PartialTimeDerivative( fn ), min( min ), max( max ), step( step )
    {
        u_long n_Segments = (u_long)( ( max - min ) / step ) + 1;
        for ( u_long i = 0; i < n_Segments; ++i )
        {
            AddElement( new Cube(
                vec3( -step / 2 ),
                vec3(  step / 2 ),
                new Transform( vec3( i*step+min, InitialState( i * step + min )), glm::identity<quat>(), glm::one<vec3>() ),
                tex->FindLocalTexture( w ),
                w
            ) );
        }
    }

    struct FourierConstants
    {
        uint N;
        float T;
        vector<vec2> a_n;
        vector<vec2> b_n;
    };
    FourierConstants FourierApproximation( uint N );
    FourierConstants FourierDerivative( FourierConstants f );
    StaticFunction *FourierFnApprox( FourierConstants f, float min, float max, GlobalTexture *texture, Window *window );


    virtual void Render()
    {
        delete[] PreviousStateSave;
        PreviousStateSave = new vec3[ Elements.size() ];

        bool smoothed = false;
        for ( u_long i = 0; i < Elements.size(); ++i )
        {
            PreviousStateSave[ i ] = Elements[ i ]->transform->pos;
            PreviousStateSave[ i ].x = i * step + min;
        }

        //calculate the function as a fourier series
        FourierConstants f0 = FourierApproximation( 100 );
        FourierConstants f1 = FourierDerivative( f0 );
        FourierConstants f2 = FourierDerivative( f1 );

        auto FourierSeries = [] ( FourierConstants f, float x ) {
            vec2 val = f.a_n[ 0 ];
            for ( int n = 1; n < f.N; ++n )
                val += f.a_n[ n ] * glm::cos( glm::pi<float>() * n / f.T * x ) + f.b_n[ n ] * glm::sin( glm::pi<float>() * n / f.T * x );
            return val;
        };

        for ( u_long i = 0; i < Elements.size(); ++i )
        {
            vec2 pfpx = FourierSeries( f1, min + i*step );
            vec2 p2fpx2 = FourierSeries( f2, min + i*step );

            vec2 f = vec2( PreviousStateSave[ i ].y, PreviousStateSave[ i ].z );

            //vec2 dv_0 = PartialTimeDerivative( f, pfpx, p2fpx2,  i * step + min, DynamicFunction::FunctionTime ) * FunctionDeltaTime;
            //vec2 dv_1 = PartialTimeDerivative( f, pfpx, p2fpx2,  i * step + min + step / 2.f, DynamicFunction::FunctionTime ) * FunctionDeltaTime;
            //vec2 dv_2 = PartialTimeDerivative( f, pfpx, p2fpx2,  i * step + min - step / 2.f, DynamicFunction::FunctionTime ) * FunctionDeltaTime;
            vec2 dv = PartialTimeDerivative( f, pfpx, p2fpx2, i*step+min, DynamicFunction::FunctionTime ) * FunctionDeltaTime;

            //Elements[ i ]->transform->pos += (1.f/6) * vec3( 0, dv_0 ) + (2.f/3) * vec3( 0, dv_1 ) + (1.f/6) * vec3( 0, dv_2 );
            Elements[ i ]->transform->pos = vec3( min + i*step, dv + FourierSeries( f0, min + i*step ) );
        }
        Element::Render();
    }

    vec3 *PreviousStateSave = NULL;

    // pf/pt = F[ f, pf/px, p2f/px2, x, t ]
    // consider up to 2nd order differential equations (for now)
    function<vec2( vec2, vec2, vec2, float, float )> PartialTimeDerivative;
    float min, max, step;

    static inline float FunctionDeltaTime;
};

#endif
