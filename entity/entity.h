#ifndef ENTITY_H
#define ENTITY_H

#pragma once

#include "../mesh/mesh.h"

struct Entity
{
    Entity( Mesh *sides[ 6 ], Transform transform );
    Entity( glm::vec3 mins, glm::vec3 maxs, Transform transform, Texture *texture );
    Entity( glm::vec3 mins, glm::vec3 maxs, Transform transform, Texture *textures[ 6 ] );
    ~Entity();

    Mesh *sides[ 6 ];

    Mesh *front     = sides[ 0 ];
    Mesh *back      = sides[ 1 ];
    Mesh *left      = sides[ 2 ];
    Mesh *right     = sides[ 3 ];
    Mesh *top       = sides[ 4 ];
    Mesh *bottom    = sides[ 5 ];

    Transform transform;

    void Render( Shader *shader );
};

#endif