#ifndef MATHS_UTIL_H
#define MATHS_UTIL_H

#include <vector>
using std::vector;

#include <string>

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
    void operator /= ( complex c )
    {
        complex t = *this / c;
        real = t.real;
        imag = t.imag;
    }
    void operator /= ( float f )
    {
        real /= f;
        imag /= f;
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

struct RCPoint
{
    RCPoint( float x, complex y ) : x( x ), y( y ) {}
    RCPoint( vec3 v ) : x( v.x ), y( complex( v.y, v.z ) ) {}
    RCPoint() : x( 0.f ), y( complex( 0.f, 0.f ) ) {}
    float x;
    complex y;

    vec3 to_vec3()
    {
        return vec3( x, y.real, y.imag );
    }
};

vector<vec3> to_vector_form( vector<RCPoint> x );
vector<RCPoint> to_complex_form( vector<vec3> x );
vector<RCPoint> to_complex_form( vec3 *x, int N );
vector<RCPoint> fft( vector<RCPoint> x );
vector<RCPoint> ifft( vector<float> domain, vector<RCPoint> X );
vector<RCPoint> FourierSpaceDerivative( vector<float> domain, vector<RCPoint> X );
#endif