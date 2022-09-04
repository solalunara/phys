#ifndef ENTITY_H
#define ENTITY_H

#pragma once

#include "mesh.h"
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

struct Entity
{
    Entity( vec3 mins, vec3 maxs, Transform transform, Texture *texture, Window *container );
    Entity( vec3 mins, vec3 maxs, Transform transform, Texture *textures[ 6 ], Window *container );
    ~Entity();

    Window *container;

    Mesh *sides[ 6 ];

    Mesh *front     = sides[ 0 ];
    Mesh *back      = sides[ 1 ];
    Mesh *left      = sides[ 2 ];
    Mesh *right     = sides[ 3 ];
    Mesh *top       = sides[ 4 ];
    Mesh *bottom    = sides[ 5 ];

    Texture *front_tex     = sides[ 0 ]->texture;
    Texture *back_tex      = sides[ 1 ]->texture;
    Texture *left_tex      = sides[ 2 ]->texture;
    Texture *right_tex     = sides[ 3 ]->texture;
    Texture *top_tex       = sides[ 4 ]->texture;
    Texture *bottom_tex    = sides[ 5 ]->texture;

    Transform transform;
    vec3 velocity;

    void Render();
};

struct Word
{
    Word( const char *text, float x, float y, float z, float scale, vec3 color, Window *container );
    ~Word();

    Window *container;
    vector<Text *> chars;
};


#endif