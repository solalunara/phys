#include "function.h"
#include <string>

DifferentialFunction::FourierConstants DifferentialFunction::FourierDerivative( FourierConstants f )
{
    FourierConstants z = FourierConstants();
    z.N = f.N;
    z.T = f.T;
    z.a_n = vector<vec2>( z.N );
    z.b_n = vector<vec2>( z.N );

    //first derivative
    // b_n = -a_n * 2pi n / T
    // a_n =  b_n * 2pi n / T
    for ( int j = 0; j < z.N; ++j )
    {
        z.b_n[ j ] = -f.a_n[ j ] * 2.f * glm::pi<float>() * (float)j / z.T;
        z.a_n[ j ] =  f.b_n[ j ] * 2.f * glm::pi<float>() * (float)j / z.T;
    }
    return z;
}

DifferentialFunction::FourierConstants DifferentialFunction::FourierApproximation( uint N )
{
    DifferentialFunction::FourierConstants f = FourierConstants();
    f.N = N;
    f.T = max - min;

    f.a_n = vector<vec2>( f.N );
    f.b_n = vector<vec2>( f.N );

    //a_n = 2/T int -T/2 T/2 of f(x) cos[ 2 pi n / T * x ] dx
    //a_n = 2/T sum 0 to N of f_i cos[ 2 pi n / T * (min+i*step)] * step
    for ( int n = 0; n < N; ++n )
    {
        vec2 an = glm::zero<vec2>();
        for ( int i = 0; i < Elements.size(); ++i )
        {
            vec3 an3 = PreviousStateSave[ i ] * glm::cos( 2.f * glm::pi<float>() * n / f.T * (min+i*step) );
            an += vec2( an3.y, an3.z ) * step * 2.f / f.T;
        }
        f.a_n[ n ] = an;

        vec2 bn = glm::zero<vec2>();
        for ( int i = 0; i < Elements.size(); ++i )
        {
            vec3 bn3 = PreviousStateSave[ i ] * glm::sin( 2.f * glm::pi<float>() * n / f.T * (min+i*step) );
            bn += vec2( bn3.y, bn3.z ) * step * 2.f / f.T;
        }
        f.b_n[ n ] = bn;
    }

    return f;
}

StaticFunction *DifferentialFunction::FourierFnApprox( FourierConstants f, float min, float max, GlobalTexture *texture, Window *window )
{
    auto FourierSeries = [ f ] ( float x ) {
        vec2 val = f.a_n[ 0 ] / 2.f;
        for ( int n = 1; n < f.N; ++n )
            val += f.a_n[ n ] * glm::cos( 2.f * glm::pi<float>() * n / f.T * x ) + f.b_n[ n ] * glm::sin( 2.f * glm::pi<float>() * n / f.T * x );
        return val;
    };
    return new StaticFunction( min, max, FourierSeries, new Transform( glm::zero<vec3>(), glm::identity<quat>(), glm::one<vec3>() ), texture, window );
}