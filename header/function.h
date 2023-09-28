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
    StaticFunction( float min, float max, function<vec3( float )> fn, Transform *transform, GlobalTexture *tex, Window *w, float step = 0.03f ) :
        GameElement( w, transform )
    {
        u_long n_Segments = (u_long)( ( max - min ) / step ) + 1;
        for ( u_long i = 0; i < n_Segments; ++i )
        {
            AddElement( new Cube(
                fn( -step / 2 + i * step + min ) - fn( i * step + min ),
                fn(  step / 2 + i * step + min ) - fn( i * step + min ),
                new Transform( fn( i * step + min ), glm::identity<quat>(), glm::one<vec3>() ),
                tex->FindLocalTexture( w ),
                w
            ) );
        }
    }
};

struct DynamicFunction :
    public GameElement
{
    DynamicFunction( float min, float max, function<vec3( float, float )> fn, Transform *transform, GlobalTexture *tex, Window *w, float step = 0.03f ) :
        GameElement( w, transform ), fn( fn ), min( min ), step( step )
    {
        u_long n_Segments = (u_long)( ( max - min ) / step ) + 1;
        for ( u_long i = 0; i < n_Segments; ++i )
        {
            AddElement( new Cube(
                vec3( -step / 2 ),
                vec3(  step / 2 ),
                new Transform( fn( i * step + min, 0 ), glm::identity<quat>(), glm::one<vec3>() ),
                tex->FindLocalTexture( w ),
                w
            ) );
        }
    }

    virtual void Render()
    {
        for ( u_long i = 0; i < Elements.size(); ++i )
            Elements[ i ]->transform->pos = fn( i * step + min, FunctionTime );
        Element::Render();
    }

    function<vec3( float, float )> fn;
    float min, step;

    static inline float FunctionTime;
};

// f = f[x, dx, t, dt]
struct DifferentialFunction :
    public GameElement
{
    DifferentialFunction( float min, float max, function<vec3( float )> InitialState, function<vec3( float, float, float, float )> fn, Transform *transform, GlobalTexture *tex, Window *w, float step = 0.03f ) :
        GameElement( w, transform ), PreviousState( InitialState ), fn( fn ), min( min ), max( max ), step( step )
    {
        u_long n_Segments = (u_long)( ( max - min ) / step ) + 1;
        for ( u_long i = 0; i < n_Segments; ++i )
        {
            AddElement( new Cube(
                vec3( -step / 2 ),
                vec3(  step / 2 ),
                new Transform( InitialState( i * step + min ), glm::identity<quat>(), glm::one<vec3>() ),
                tex->FindLocalTexture( w ),
                w
            ) );
        }
    }

    virtual void Render()
    {
        PreviousStateSave = new vec3[ Elements.size() ];
        for ( u_long i = 0; i < Elements.size(); ++i )
        {
            PreviousStateSave[ i ] = Elements[ i ]->transform->pos;
            PreviousStateSave[ i ].x = i * step + min;
        }
        PreviousState = [ this ] ( float x ) { if ( x >= min && x <= max ) return PreviousStateSave[ (u_long)( ( x - min ) / step ) ]; else return vec3( x, 0, 0 ); };

        for ( u_long i = 0; i < Elements.size(); ++i )
            Elements[ i ]->transform->pos = fn( i * step + min, step, DynamicFunction::FunctionTime, FunctionDeltaTime );
        Element::Render();
    }

    function<vec3( float )> PreviousState;
    vec3 *PreviousStateSave;
    function<vec3( float, float, float, float )> fn;
    float min, max, step;

    static inline float FunctionDeltaTime;
};

#endif
