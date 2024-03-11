#include "maths_util.h"

vector<vec3> to_vector_form( vector<RCPoint> x )
{
    vector<vec3> ret_val;
    for ( int i = 0; i < x.size(); ++i )
        ret_val.push_back( vec3( x[ i ].x, x[ i ].y.real, x[ i ].y.imag ) );
    return ret_val;
}
vector<RCPoint> to_complex_form( vector<vec3> x )
{
    vector<RCPoint> ret_val;
    for ( int i = 0; i < x.size(); ++i )
        ret_val.push_back( RCPoint( x[ i ].x, complex( x[ i ].y, x[ i ].z ) ) );
    return ret_val;
}
vector<RCPoint> to_complex_form( vec3 *x, int N )
{
    vector<RCPoint> ret_val;
    for ( int i = 0; i < N; ++i )
        ret_val.push_back( RCPoint( x[ i ].x, complex( x[ i ].y, x[ i ].z ) ) );
    return ret_val;
}

vector<RCPoint> fft( vector<RCPoint> x, int N, int s )
{
    if ( N == 1 )
        return x;

    vector<RCPoint> even( N / 2 );
    vector<RCPoint> odd( N / 2 );
    for ( int i = 0; i < N / 2; ++i )
    {
        even[ i ] = x[ i * 2 ];
        odd[ i ] = x[ i * 2 + 1 ];
    }

    even = fft( even, N / 2, s * 2 );
    odd = fft( odd, N / 2, s * 2 );

    vector<RCPoint> X( N );
    for ( int k = 0; k < N / 2; ++k )
    {
        complex t = complex( cos( -2.f * glm::pi<float>() * x[ k ].x ), sin( -2.f * glm::pi<float>() * x[ k ].x ) ) * odd[ k ].y;
        X[ k ] = RCPoint( x[ k ].x, even[ k ].y + t );
        X[ k + N / 2 ] = RCPoint( x[ k + N / 2 ].x, even[ k ].y - t );
    }
    return X;
}

vector<RCPoint> fft( vector<RCPoint> x )
{
    int N = x.size();
    int M = 1;
    while ( M < N ) M <<= 1; // find the smallest power of two greater than N

    x.resize( M, RCPoint( 0, 0 ) ); // resize the vector to M, padding with zeros if necessary

    vector<RCPoint> ret_val_complex = fft( x, M, 1 );
    ret_val_complex.resize( N );
    return ret_val_complex;
}

vector<RCPoint> ifft( vector<RCPoint> X, int N, int s )
{
    if ( N == 1 )
        return X;

    vector<RCPoint> even( N / 2 );
    vector<RCPoint> odd( N / 2 );
    for ( int i = 0; i < N / 2; ++i )
    {
        even[ i ] = X[ i * 2 ];
        odd[ i ] = X[ i * 2 + 1 ];
    }

    even = ifft( even, N / 2, s * 2 );
    odd = ifft( odd, N / 2, s * 2 );

    vector<RCPoint> x( N );
    for ( int k = 0; k < N / 2; ++k )
    {
        complex t = complex( cos( 2.f * glm::pi<float>() * X[ k ].x ), sin( 2.f * glm::pi<float>() * X[ k ].x ) ) * odd[ k ].y;
        x[ k ] = RCPoint( X[ k ].x, even[ k ].y + t );
        x[ k + N / 2 ] = RCPoint( X[ k ].x, even[ k ].y - t );
    }
    return x;
}

vector<RCPoint> ifft( vector<RCPoint> X )
{
    int N = X.size();
    int M = 1;
    while ( M < N ) M <<= 1; // find the smallest power of two greater than N

    X.resize( M, RCPoint( 0, 0 ) ); // resize the vector to M, padding with zeros if necessary

    vector<RCPoint> ret_val_complex = ifft( X, M, 1 );
    ret_val_complex.resize( N );
    return ret_val_complex;
}

//takes x and X as input, where x is the domain and X is the function in the Fourier domain
vector<RCPoint> FourierSpaceDerivative( vector<RCPoint> X )
{
    int N = X.size();
    vector<RCPoint> X_prime( N );
    for ( int n = 0; n < N; ++n )
        X_prime[ n ] = RCPoint( X[ n ].x, X[ n ].y * complex( 0.f, 2.f * glm::pi<float>() * X[ n ].x ) );
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