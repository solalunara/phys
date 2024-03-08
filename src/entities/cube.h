#ifndef ENTITY_H
#define ENTITY_H

#pragma once

#include "render/mesh.h"
#include "element.h"
#include <vector>
#include <glm/glm.hpp>
using std::vector;
using glm::vec2;
using glm::vec3;
using glm::quat;

struct Window;
struct Text;
struct GlobalTexture;

enum class Side : char
{
    NONE = 0,
    front = 1 << 0,
    back = 1 << 1,
    left = 1 << 2,
    right = 1 << 3,
    top = 1 << 4,
    bottom = 1 << 5,
};

struct TexSide
{
    Texture *texture;
    Side side;
};

struct Cube :
    public GameElement
{
    Cube( vec3 mins, vec3 maxs, Transform *transform, Texture *texture, Window *container );
    Cube( vec3 mins, vec3 maxs, Transform *transform, Texture *textures[ 6 ], Window *container );
    Cube( vec3 mins, vec3 maxs, Transform *transform, GlobalTexture *texture, Window *container );
    Cube( vec3 mins, vec3 maxs, Transform *transform, GlobalTexture *textures[ 6 ], Window *container );

    //Mesh *sides[ 6 ];

    /*
    Mesh *front     = sides[ 0 ];
    Mesh *back      = sides[ 1 ];
    Mesh *left      = sides[ 2 ];
    Mesh *right     = sides[ 3 ];
    Mesh *top       = sides[ 4 ];
    Mesh *bottom    = sides[ 5 ];
    */

    Texture *front_tex;
    Texture *back_tex;
    Texture *left_tex;
    Texture *right_tex;
    Texture *top_tex;
    Texture *bottom_tex;

private:
    vec3 mins;
    vec3 maxs;
};

#endif