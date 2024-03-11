#ifndef MATHS_UTIL_H
#define MATHS_UTIL_H

#include <vector>
using std::vector;

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
using glm::vec2;
using glm::vec3;

struct complex
{
    complex( float r, float i ) : real( r ), imag( i ) {}
    complex( vec2 v ) : real( v.x ), imag( v.y ) {}
    complex( float r ) : real( r ), imag( 0.f ) {}
    complex() : real( 0.f ), imag( 0.f ) {}

    float real;
    float imag;

    float magnitude()
    {
        return sqrt( real * real + imag * imag );
    }
    float argument()
    {
        return atan2( imag, real );
    }

    complex conjugate()
    {
        return complex( real, -imag );
    }

    complex operator =( float f )
    {
        return complex( f, 0.f );
    }

    complex operator +( float f )
    {
        return complex( real + f, imag );
    }
    complex operator -( float f )
    {
        return complex( real - f, imag );
    }

    complex operator +( complex c )
    {
        return complex( real + c.real, imag + c.imag );
    }
    complex operator -( complex c )
    {
        return complex( real - c.real, imag - c.imag );
    }
    complex operator *( complex c )
    {
        return complex( real * c.real - imag * c.imag, real * c.imag + imag * c.real );
    }
    complex operator /( complex c )
    {
        float r = c.real * c.real + c.imag * c.imag;
        return complex( ( real * c.real + imag * c.imag ) / r, ( imag * c.real - real * c.imag ) / r );
    }
    bool operator ==( complex c )
    {
        return real == c.real && imag == c.imag;
    }
    bool operator !=( complex c )
    {
        return real != c.real || imag != c.imag;
    }
};

vector<vec2> to_vector_form( vector<complex> x )
{
    vector<vec2> ret_val;
    for ( int i = 0; i < x.size(); ++i )
        ret_val.push_back( vec2( x[ i ].real, x[ i ].imag ) );
    return ret_val;
}
vector<complex> to_complex_form( vector<vec2> x )
{
    vector<complex> ret_val;
    for ( int i = 0; i < x.size(); ++i )
        ret_val.push_back( complex( x[ i ].x, x[ i ].y ) );
    return ret_val;
}
vector<complex> to_complex_form( vector<vec3> x )
{
    vector<complex> ret_val;
    for ( int i = 0; i < x.size(); ++i )
        ret_val.push_back( complex( x[ i ].y, x[ i ].z ) );
    return ret_val;
}
vector<complex> to_complex_form( vec3 *x, int N )
{
    vector<complex> ret_val;
    for ( int i = 0; i < N; ++i )
        ret_val.push_back( complex( x[ i ].y, x[ i ].z ) );
    return ret_val;
}

vector<complex> fft( vector<complex> x, int N, int s )
{
    if ( N == 1 )
        return x;

    vector<complex> even( N / 2 );
    vector<complex> odd( N / 2 );
    for ( int i = 0; i < N / 2; ++i )
    {
        even[ i ] = x[ i * 2 ];
        odd[ i ] = x[ i * 2 + 1 ];
    }

    even = fft( even, N / 2, s * 2 );
    odd = fft( odd, N / 2, s * 2 );

    vector<complex> X( N );
    for ( int k = 0; k < N / 2; ++k )
    {
        complex t = complex( cos( -2.f * glm::pi<float>() * k / N ), sin( -2.f * glm::pi<float>() * k / N ) ) * odd[ k ];
        X[ k ] = even[ k ] + t;
        X[ k + N / 2 ] = even[ k ] - t;
    }
    return X;
}

vector<complex> fft( vector<complex> x )
{
    int N = x.size();
    int M = 1;
    while ( M < N ) M <<= 1; // find the smallest power of two greater than N

    x.resize( M, 0 ); // resize the vector to M, padding with zeros if necessary
    vector<complex> x_complex( M );
    for ( int i = 0; i < M; ++i )
        x_complex[ i ] = complex( x[ i ] );

    vector<complex> ret_val_complex = fft( x_complex, M, 1 );
    return ret_val_complex;
}

vector<complex> ifft( vector<complex> X, int N, int s )
{
    if ( N == 1 )
        return X;

    vector<complex> even( N / 2 );
    vector<complex> odd( N / 2 );
    for ( int i = 0; i < N / 2; ++i )
    {
        even[ i ] = X[ i * 2 ];
        odd[ i ] = X[ i * 2 + 1 ];
    }

    even = ifft( even, N / 2, s * 2 );
    odd = ifft( odd, N / 2, s * 2 );

    vector<complex> x( N );
    for ( int k = 0; k < N / 2; ++k )
    {
        complex t = complex( cos( 2.f * glm::pi<float>() * k / N ), sin( 2.f * glm::pi<float>() * k / N ) ) * odd[ k ];
        x[ k ] = even[ k ] + t;
        x[ k + N / 2 ] = even[ k ] - t;
    }
    return x;
}

vector<complex> ifft( vector<complex> X )
{
    int N = X.size();
    int M = 1;
    while ( M < N ) M <<= 1; // find the smallest power of two greater than N

    X.resize( M, 0 ); // resize the vector to M, padding with zeros if necessary

    vector<complex> ret_val_complex = ifft( X, M, 1 );
    return ret_val_complex;
}

//takes x and X as input, where x is the domain and X is the function in the Fourier domain
vector<complex> FourierSpaceDerivative( vector<float> x, vector<complex> X )
{
    int N = X.size();
    if ( x.size() != N )
        throw "FourierSpaceDerivative: x and X must have the same size";
    vector<complex> X_prime( N );
    for ( int n = 0; n < N; ++n )
        X_prime[ n ] = X[ n ] * complex( 0.f, 2.f * glm::pi<float>() * x[ n ] );
    return X_prime;
}

vector<vec3> FourierSpaceDerivative( vector<vec3> fn )
{
    int N = fn.size();
    vector<vec3> X_prime( N );
    for ( int n = 0; n < N; ++n )
        X_prime[ n ] = vec3( fn[ n ].x, -fn[ n ].z * 2.f * glm::pi<float>() * fn[ n ].x, fn[ n ].y * 2.f * glm::pi<float>() * fn[ n ].x );
    return X_prime;
}

#endif