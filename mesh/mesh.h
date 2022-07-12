#ifndef MESH_H
#define MESH_H

#pragma once

#include "../transform.h"

struct Texture;
struct Shader;

struct Mesh
{
    Mesh( float *verts, unsigned long long verts_len, unsigned int *inds, unsigned long long inds_len, Texture *texture, Transform transform );
    ~Mesh();

    Mesh( const Mesh & ) = delete;
    Mesh &operator =( const Mesh & ) = delete;

    Mesh( Mesh &&other ) : _VBO( other._VBO ), _VAO( other._VAO ), _EBO( other._EBO ), transform( other.transform )
    {
        other._VBO = 0;
        other._VAO = 0;
        other._EBO = 0;
    }

    void Render( Shader *shader );

    Texture *texture;
    Transform transform;

private:
    unsigned int _VBO;
    unsigned int _VAO;
    unsigned int _EBO;

    float *verts;
    unsigned long long verts_len;

    unsigned int *inds;
    unsigned long long inds_len;
};

#endif