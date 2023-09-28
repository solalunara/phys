#include "axis.h"
#include "cube.h"
#include "text.h"
#include "GlobalTexture.h"
#include "window.h"
#include <string>

Axis::Axis( unsigned char CartnBasisVector, vec3 Origin, GlobalTexture *tex, int Length, int Step, Window *container ) :
    dir( CartnBasisVector ), Length( Length ), Step( Step )
{
    unsigned long long BoxNum = 2 * Length / Step;
    switch ( CartnBasisVector )
    {
        case i_hat:
        AxisLabel = new GameText( "x", NULL, Origin.x + Length / 2.0f, Origin.y - .7f, Origin.z, 0.01, vec3( .8f, .8f, .8f ), container );
        break;

        case j_hat:
        AxisLabel = new GameText( "y", NULL, Origin.x - .7f, Origin.y + Length / 2.0f, Origin.z, 0.01, vec3( .8f, .8f, .8f ), container );
        break;

        case k_hat:
        AxisLabel = new GameText( "z", NULL, Origin.x, Origin.y - .7f, Origin.z + Length / 2.0f, 0.01, vec3( .8f, .8f, .8f ), container );
        AxisLabel->transform->rot = glm::angleAxis( glm::radians( -90.0f ), vec3( 0, 1, 0 ) );
        break;
    }
#define NUMLABEL_OFFSET .3f
    for ( int i = 0; i < BoxNum; ++i )
    {
        switch ( CartnBasisVector )
        {
            case i_hat:
            boxes.push_back( new Cube( vec3( -(float)Step / 2.0f, -.1f, -.1f ), vec3( (float)Step / 2.0f, .1f, .1f ), 
                new Transform( Origin + vec3( (float)i * (float)Step - Length, 0, 0 ), glm::identity<quat>(), vec3( 1.f, 1.f, 1.f ) ),
                tex->FindLocalTexture( container ), container ) );

            ValueLabels.push_back( new GameText( std::to_string( i * Step - Length ).c_str(), NULL, Origin.x + i * Step - Length, Origin.y - NUMLABEL_OFFSET, Origin.z, 0.01, vec3( 1, 1, 1 ), container ) );
            break;

            case j_hat:
            boxes.push_back( new Cube( vec3( -.1f, -(float)Step / 2.0f, -.1f ), vec3( .1f, (float)Step / 2.0f, .1f ), 
                new Transform( Origin + vec3( 0, (float)i * (float)Step - Length, 0 ), glm::identity<quat>(), vec3( 1.f, 1.f, 1.f ) ),
                tex->FindLocalTexture( container ), container ) );

            ValueLabels.push_back( new GameText( std::to_string( i * Step - Length ).c_str(), NULL, Origin.x - NUMLABEL_OFFSET, Origin.y + i * Step - Length, Origin.z, 0.01, vec3( 1, 1, 1 ), container ) );
            break;

            case k_hat:
            boxes.push_back( new Cube( vec3( -.1f, -.1f, -(float)Step / 2.0f ), vec3( .1f, .1f, (float)Step / 2.0f ), 
                new Transform( Origin + vec3( 0, 0, (float)i * (float)Step - Length ), glm::identity<quat>(), vec3( 1.f, 1.f, 1.f ) ),
                tex->FindLocalTexture( container ), container ) );

            ValueLabels.push_back( new GameText( std::to_string( i * Step - Length ).c_str(), NULL, Origin.x, Origin.y - NUMLABEL_OFFSET, Origin.z + i * Step - Length, 0.01, vec3( 1, 1, 1 ), container ) );
            //rot the labels so they face out from the z axis
            ValueLabels[ ValueLabels.size() - 1 ]->transform->rot = glm::angleAxis( glm::radians( -90.0f ), vec3( 0, 1, 0 ) );
            break;
        }
    }
}
Axis::Axis( unsigned char PolarBasisVector, vec3 Origin, GlobalTexture *tex, int Length, int Step, int OtherValue, Window *container ) :
    dir( PolarBasisVector ), Length( Length ), Step( Step )
{

}