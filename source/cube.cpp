#include "cube.h"
#include "window.h"
#include "GlobalTexture.h"
#include <string.h>

Cube::Cube( vec3 mins, vec3 maxs, Transform &&transform, Texture *texture, Window *container ) :
    Cube( mins, maxs, (Transform &&)transform, new Texture *[ 6 ] { texture, texture, texture, texture, texture, texture }, container )
{
}
Cube::Cube( vec3 mins, vec3 maxs, Transform &&transform, Texture *textures[ 6 ], Window *container ) :
    sides{ 
        new Mesh( vec2( mins.x, mins.y ), vec2( maxs.x, maxs.y ), textures[ 0 ], Transform( vec3( 0, 0, maxs.z ), glm::identity<quat>(), glm::one<vec3>() ), container ),
        new Mesh( vec2( mins.x, mins.y ), vec2( maxs.x, maxs.y ), textures[ 1 ], Transform( vec3( 0, 0, mins.z ), glm::angleAxis(  M_PIf  , vec3( 0, 1, 0 ) ), glm::one<vec3>() ), container ),
        new Mesh( vec2( mins.z, mins.y ), vec2( maxs.z, maxs.y ), textures[ 2 ], Transform( vec3( mins.x, 0, 0 ), glm::angleAxis( -M_PI_2f, vec3( 0, 1, 0 ) ), glm::one<vec3>() ), container ),
        new Mesh( vec2( mins.z, mins.y ), vec2( maxs.z, maxs.y ), textures[ 3 ], Transform( vec3( maxs.x, 0, 0 ), glm::angleAxis( -M_PI_2f, vec3( 0, 1, 0 ) ), glm::one<vec3>() ), container ),
        new Mesh( vec2( mins.z, mins.x ), vec2( maxs.z, maxs.x ), textures[ 4 ], Transform( vec3( 0, maxs.y, 0 ), glm::angleAxis( -M_PI_2f, vec3( 1, 0, 0 ) ), glm::one<vec3>() ), container ),
        new Mesh( vec2( mins.z, mins.x ), vec2( maxs.z, maxs.x ), textures[ 5 ], Transform( vec3( 0, mins.y, 0 ), glm::angleAxis(  M_PI_2f, vec3( 1, 0, 0 ) ), glm::one<vec3>() ), container ),
    }, GameElement( container, (Transform &&)transform )
{
    for ( int i = 0; i < 6; ++i )
        AddElement( this->sides[ i ] );
}