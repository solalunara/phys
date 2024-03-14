#include "maths_util.h"

vector<RCPoint> applyWindow(vector<RCPoint> x) {
    int N = x.size();
    vector<RCPoint> windowed(N);
    for (int i = 0; i < N; ++i) {
        float windowValue = (1 - cos(2 * glm::pi<float>() * i / (N - 1))); // times .5?
        windowed[i] = RCPoint(i, x[i].y * windowValue);
    }
    return windowed;
}


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

vector<RCPoint> cooley_tukey( vector<RCPoint> x, int N, int s )
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

    even = cooley_tukey( even, N / 2, s * 2 );
    odd = cooley_tukey( odd, N / 2, s * 2 );

    vector<RCPoint> X( N );
    for ( int k = 0; k < N / 2; ++k )
    {
        complex t = complex( cos( -2.f * glm::pi<float>() * k / N ), sin( -2.f * glm::pi<float>() * k / N ) ) * odd[ k ].y;
        X[ k ] = RCPoint( k, even[ k ].y + t );
        X[ k + N / 2 ] = RCPoint( k + N / 2, even[ k ].y - t );
    }
    return X;
}


vector<RCPoint> fft( vector<RCPoint> x )
{
    int N = x.size();
    int M = 1;
    while ( M <= N ) M <<= 1; // find the smallest power of two greater than or equal to N
    M >>= 1;

    x.resize( M );

    vector<RCPoint> ret_val_complex = cooley_tukey( x, M, 1 );
    return ret_val_complex;
}

vector<RCPoint> inverse_cooley_tuckey( vector<float> domain, vector<RCPoint> X, int N, int s )
{
    if ( N == 1 )
        return X;

    vector<RCPoint> even_cpx( N / 2 );
    vector<float> even_domain( N / 2 );
    vector<RCPoint> odd_cpx( N / 2 );
    vector<float> odd_domain( N / 2 );
    for ( int i = 0; i < N / 2; ++i )
    {
        even_cpx[ i ] = X[ i * 2 ];
        even_domain[ i ] = domain[ i * 2 ];
        odd_cpx[ i ] = X[ i * 2 + 1 ];
        odd_domain[ i ] = domain[ i * 2 + 1 ];
    }

    vector<RCPoint> even = inverse_cooley_tuckey( even_domain, even_cpx, N / 2, s * 2 );
    vector<RCPoint> odd = inverse_cooley_tuckey( odd_domain, odd_cpx, N / 2, s * 2 );

    vector<RCPoint> x( N );
    for ( int k = 0; k < N / 2; ++k )
    {
        complex t = complex( cos( 2.f * glm::pi<float>() * k / N ), sin( 2.f * glm::pi<float>() * k / N ) ) * odd[ k ].y;
        x[ k ] = RCPoint( domain[ k ], even[ k ].y + t );
        x[ k + N / 2 ] = RCPoint( domain[ k + N / 2 ], even[ k ].y - t );
    }
    return x;
}

vector<RCPoint> ifft( vector<float> domain, vector<RCPoint> X )
{
    int N = X.size();
    int M = 1;
    while ( M <= N ) M <<= 1; // find the smallest power of two greater than or equal to N
    M >>= 1;

    domain.resize( M );
    X.resize( M );

    vector<RCPoint> ret_val_complex = inverse_cooley_tuckey( domain, X, M, 1 );
    for ( int k = 0; k < N; ++k )
        ret_val_complex[ k ].y /= M;
    return ret_val_complex;
}

//takes x and X as input, where x is the domain and X is the function in the Fourier domain
vector<RCPoint> FourierSpaceDerivative( vector<float> domain, vector<RCPoint> X )
{
    float T = domain[ X.size() - 1 ] - domain[ 0 ];
    int N = X.size();
    vector<RCPoint> X_prime( N );
    for ( int n = 0; n < N; ++n )
        X_prime[ n ] = RCPoint( X[ n ].x, X[ n ].y * complex( 0.f, 2.f * glm::pi<float>() * domain[ n ] ) );
    return X_prime;
}