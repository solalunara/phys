#ifndef FUNCTION_H
#define FUNCTION_H

#pragma once

#include "entities/cube.h"
#include "maths_util.h"
#include <glm/vec3.hpp>
#include <vector>
#include <functional>
#include <cstdio>
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
            AddChild( new Cube(
                vec3( -step / 2 ),
                vec3(  step / 2 ),
                new Transform( vec3( i*step+min, fn( i * step + min ) ), glm::identity<quat>(), glm::one<vec3>() ),
                tex->FindLocalTexture( w ),
                w
            ) );
        }
    }

    StaticFunction( vector<vec3> pts, Transform *transform, GlobalTexture *tex, Window *w, float step = 0.03f ) :
        GameElement( w, transform )
    {
        u_long n_Segments = (u_long)( pts.size() );
        for ( u_long i = 0; i < n_Segments; ++i )
        {
            AddChild( new Cube(
                vec3( -step / 2 ),
                vec3(  step / 2 ),
                new Transform( pts[ i ], glm::identity<quat>(), glm::one<vec3>() ),
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
            AddChild( new Cube(
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
            AddChild( new Cube(
                vec3( -step / 2 ),
                vec3(  step / 2 ),
                new Transform( vec3( i*step+min, InitialState( i * step + min )), glm::identity<quat>(), glm::one<vec3>() ),
                tex->FindLocalTexture( w ),
                w
            ) );
        }
    }
    ~DifferentialFunction()
    {
        delete[] PreviousStateSave;
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

    vector<float> GetDomain()
    {
        vector<float> x = vector<float>( Elements.size() );
        for ( u_long i = 0; i < Elements.size(); ++i )
            x[ i ] = i * step + min;
        return x;
    }
    vector<RCPoint> FourierTransform()
    {
        GenPrevState();
        vector<RCPoint> f = to_complex_form( PreviousStateSave, Elements.size() );
        return fft( f );
    }

    void GenPrevState()
    {
        delete[] PreviousStateSave;
        PreviousStateSave = new vec3[ Elements.size() ];

        bool smoothed = false;
        for ( u_long i = 0; i < Elements.size(); ++i )
        {
            PreviousStateSave[ i ] = Elements[ i ]->transform->pos;
            PreviousStateSave[ i ].x = i * step + min;
        }
    }

    virtual void Render()
    {
        GenPrevState();

        //calculate the function as a fourier series
        /*
        FourierConstants f0 = FourierApproximation( 100 );
        FourierConstants f1 = FourierDerivative( f0 );
        FourierConstants f2 = FourierDerivative( f1 );

        auto FourierSeries = [] ( FourierConstants f, float x ) {
            vec2 val = f.a_n[ 0 ] / 2.f;
            for ( int n = 1; n < f.N; ++n )
                val += f.a_n[ n ] * glm::cos( 2.f * glm::pi<float>() * n / f.T * x ) + f.b_n[ n ] * glm::sin( 2.f * glm::pi<float>() * n / f.T * x );
            return val;
        };
        */

        vector<RCPoint> f = to_complex_form( PreviousStateSave, Elements.size() );

        vector<float> x = vector<float>( Elements.size() );
        for ( u_long i = 0; i < Elements.size(); ++i )
            x[ i ] = i * step + min;

        vector<RCPoint> F0 = fft( f );
        vector<RCPoint> F1 = FourierSpaceDerivative( F0 );
        vector<RCPoint> F2 = FourierSpaceDerivative( F1 );

        vector<vec3> f0 = to_vector_form( f );
        vector<vec3> f1 = to_vector_form( ifft( F1 ) );
        vector<vec3> f2 = to_vector_form( ifft( F2 ) );

        for ( u_long i = 0; i < Elements.size(); ++i )
        {
            //fourier series approach
            /*
            vec2 pfpx_0 = FourierSeries( f1, min + i*step );
            vec2 pfpx_1 = FourierSeries( f1, min + i*step + step );
            vec2 pfpx_2 = FourierSeries( f1, min + i*step - step );
            vec2 p2fpx2_0 = FourierSeries( f2, min + i*step );
            vec2 p2fpx2_1 = FourierSeries( f2, min + i*step + step );
            vec2 p2fpx2_2 = FourierSeries( f2, min + i*step - step );

            vec2 f_0 = vec2( PreviousStateSave[ i ].y, PreviousStateSave[ i ].z );
            vec2 f_1 = FourierSeries( f0, min + i*step + step );
            vec2 f_2 = FourierSeries( f0, min + i*step - step );

            vec2 dv_0 = PartialTimeDerivative( f_0, pfpx_0, p2fpx2_0,  i * step + min, DynamicFunction::FunctionTime ) * FunctionDeltaTime;
            vec2 dv_1 = PartialTimeDerivative( f_1, pfpx_1, p2fpx2_1,  i * step + min + step, DynamicFunction::FunctionTime ) * FunctionDeltaTime;
            vec2 dv_2 = PartialTimeDerivative( f_2, pfpx_2, p2fpx2_2,  i * step + min - step, DynamicFunction::FunctionTime ) * FunctionDeltaTime;

            Elements[ i ]->transform->pos += (1.f/6) * vec3( 0, dv_0 ) + (2.f/3) * vec3( 0, dv_1 ) + (1.f/6) * vec3( 0, dv_2 );
            */

            //fourier transform approach
            vec2 pfpx_0 = f1[ i ];
            vec2 p2fpx2_0 = f2[ i ];

            vec2 f_0 = f0[ i ];

            vec2 dv_0 = PartialTimeDerivative( f_0, pfpx_0, p2fpx2_0,  i * step + min, DynamicFunction::FunctionTime ) * FunctionDeltaTime;

            if ( i != 0 && i != Elements.size() - 1 )
            {
                vec2 pfpx_1 = f1[ i + 1 ];
                vec2 pfpx_2 = f1[ i - 1 ];
                vec2 p2fpx2_1 = f2[ i + 1 ];
                vec2 p2fpx2_2 = f2[ i - 1 ];
                vec2 f_1 = f0[ i + 1 ];
                vec2 f_2 = f0[ i - 1 ];
                vec2 dv_1 = PartialTimeDerivative( f_1, pfpx_1, p2fpx2_1,  i * step + min + step, DynamicFunction::FunctionTime ) * FunctionDeltaTime;
                vec2 dv_2 = PartialTimeDerivative( f_2, pfpx_2, p2fpx2_2,  i * step + min - step, DynamicFunction::FunctionTime ) * FunctionDeltaTime;

                Elements[ i ]->transform->pos += (1.f/6) * vec3( 0, dv_0 ) + (2.f/3) * vec3( 0, dv_1 ) + (1.f/6) * vec3( 0, dv_2 );
            }
            else
            {
                Elements[ i ]->transform->pos += vec3( 0, dv_0 );
            }

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
