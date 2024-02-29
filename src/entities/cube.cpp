#include "cube.h"
#include "window.h"
#include "GlobalTexture.h"
#include <string.h>

Cube::Cube( vec3 mins, vec3 maxs, Transform *transform, Texture *texture, Window *container ) :
    GameElement( container, transform ), mins( mins ), maxs( maxs )
{
    AddElement( new Mesh( vec2( mins.x, mins.y ), vec2( maxs.x, maxs.y ), texture, new Transform( vec3( 0, 0, maxs.z ), glm::identity<quat>(), glm::one<vec3>() ), container ) );
    AddElement( new Mesh( vec2( mins.x, mins.y ), vec2( maxs.x, maxs.y ), texture, new Transform( vec3( 0, 0, mins.z ), glm::angleAxis(  M_PIf  , vec3( 0, 1, 0 ) ), glm::one<vec3>() ), container ) );
    AddElement( new Mesh( vec2( mins.z, mins.y ), vec2( maxs.z, maxs.y ), texture, new Transform( vec3( mins.x, 0, 0 ), glm::angleAxis(  M_PI_2f, vec3( 0, 1, 0 ) ), glm::one<vec3>() ), container ) );
    AddElement( new Mesh( vec2( mins.z, mins.y ), vec2( maxs.z, maxs.y ), texture, new Transform( vec3( maxs.x, 0, 0 ), glm::angleAxis( -M_PI_2f, vec3( 0, 1, 0 ) ), glm::one<vec3>() ), container ) );
    AddElement( new Mesh( vec2( mins.x, mins.z ), vec2( maxs.x, maxs.z ), texture, new Transform( vec3( 0, maxs.y, 0 ), glm::angleAxis(  M_PI_2f, vec3( 1, 0, 0 ) ), glm::one<vec3>() ), container ) );
    AddElement( new Mesh( vec2( mins.x, mins.z ), vec2( maxs.x, maxs.z ), texture, new Transform( vec3( 0, mins.y, 0 ), glm::angleAxis( -M_PI_2f, vec3( 1, 0, 0 ) ), glm::one<vec3>() ), container ) );

    front_tex     = texture;
    back_tex      = texture;
    left_tex      = texture;
    right_tex     = texture;
    top_tex       = texture;
    bottom_tex    = texture;

    //for ( int i = 0; i < 6; ++i )
    //    AddElement( this->sides[ i ] );
}
Cube::Cube( vec3 mins, vec3 maxs, Transform *transform, Texture *textures[ 6 ], Window *container ) :
    GameElement( container, transform ), mins( mins ), maxs( maxs )
{
    AddElement( new Mesh( vec2( mins.x, mins.y ), vec2( maxs.x, maxs.y ), textures[ 0 ], new Transform( vec3( 0, 0, maxs.z ), glm::identity<quat>(), glm::one<vec3>() ), container ) );
    AddElement( new Mesh( vec2( mins.x, mins.y ), vec2( maxs.x, maxs.y ), textures[ 1 ], new Transform( vec3( 0, 0, mins.z ), glm::angleAxis(  M_PIf  , vec3( 0, 1, 0 ) ), glm::one<vec3>() ), container ) );
    AddElement( new Mesh( vec2( mins.z, mins.y ), vec2( maxs.z, maxs.y ), textures[ 2 ], new Transform( vec3( mins.x, 0, 0 ), glm::angleAxis(  M_PI_2f, vec3( 0, 1, 0 ) ), glm::one<vec3>() ), container ) );
    AddElement( new Mesh( vec2( mins.z, mins.y ), vec2( maxs.z, maxs.y ), textures[ 3 ], new Transform( vec3( maxs.x, 0, 0 ), glm::angleAxis( -M_PI_2f, vec3( 0, 1, 0 ) ), glm::one<vec3>() ), container ) );
    AddElement( new Mesh( vec2( mins.x, mins.z ), vec2( maxs.x, maxs.z ), textures[ 4 ], new Transform( vec3( 0, maxs.y, 0 ), glm::angleAxis(  M_PI_2f, vec3( 1, 0, 0 ) ), glm::one<vec3>() ), container ) );
    AddElement( new Mesh( vec2( mins.x, mins.z ), vec2( maxs.x, maxs.z ), textures[ 5 ], new Transform( vec3( 0, mins.y, 0 ), glm::angleAxis( -M_PI_2f, vec3( 1, 0, 0 ) ), glm::one<vec3>() ), container ) );

    front_tex     = textures[ 0 ];
    back_tex      = textures[ 1 ];
    left_tex      = textures[ 2 ];
    right_tex     = textures[ 3 ];
    top_tex       = textures[ 4 ];
    bottom_tex    = textures[ 5 ];

    //for ( int i = 0; i < 6; ++i )
    //    AddElement( this->sides[ i ] );
}

Cube::Cube( vec3 mins, vec3 maxs, Transform *transform, GlobalTexture *texture, Window *container ) :
    Cube( mins, maxs, transform, texture->FindLocalTexture( container ), container )
{}

Cube::Cube( vec3 mins, vec3 maxs, Transform *transform, GlobalTexture *textures[ 6 ], Window *container ) :
    GameElement( container, transform ), mins( mins ), maxs( maxs )
{
    front_tex     = textures[ 0 ]->FindLocalTexture( container );
    back_tex      = textures[ 1 ]->FindLocalTexture( container );
    left_tex      = textures[ 2 ]->FindLocalTexture( container );
    right_tex     = textures[ 3 ]->FindLocalTexture( container );
    top_tex       = textures[ 4 ]->FindLocalTexture( container );
    bottom_tex    = textures[ 5 ]->FindLocalTexture( container );

    AddElement( new Mesh( vec2( mins.x, mins.y ), vec2( maxs.x, maxs.y ), front_tex, new Transform( vec3( 0, 0, maxs.z ), glm::identity<quat>(), glm::one<vec3>() ), container ) );
    AddElement( new Mesh( vec2( mins.x, mins.y ), vec2( maxs.x, maxs.y ), back_tex, new Transform( vec3( 0, 0, mins.z ), glm::angleAxis(  M_PIf  , vec3( 0, 1, 0 ) ), glm::one<vec3>() ), container ) );
    AddElement( new Mesh( vec2( mins.z, mins.y ), vec2( maxs.z, maxs.y ), left_tex, new Transform( vec3( mins.x, 0, 0 ), glm::angleAxis(  M_PI_2f, vec3( 0, 1, 0 ) ), glm::one<vec3>() ), container ) );
    AddElement( new Mesh( vec2( mins.z, mins.y ), vec2( maxs.z, maxs.y ), right_tex, new Transform( vec3( maxs.x, 0, 0 ), glm::angleAxis( -M_PI_2f, vec3( 0, 1, 0 ) ), glm::one<vec3>() ), container ) );
    AddElement( new Mesh( vec2( mins.x, mins.z ), vec2( maxs.x, maxs.z ), top_tex, new Transform( vec3( 0, maxs.y, 0 ), glm::angleAxis(  M_PI_2f, vec3( 1, 0, 0 ) ), glm::one<vec3>() ), container ) );
    AddElement( new Mesh( vec2( mins.x, mins.z ), vec2( maxs.x, maxs.z ), bottom_tex, new Transform( vec3( 0, mins.y, 0 ), glm::angleAxis( -M_PI_2f, vec3( 1, 0, 0 ) ), glm::one<vec3>() ), container ) );
}