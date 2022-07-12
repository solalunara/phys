#ifndef MESH_H
#define MESH_H

#pragma once

struct Texture;
struct Shader;

struct Mesh
{
    Mesh( float *verts, unsigned long long verts_len, unsigned int *inds, unsigned long long inds_len, Texture *texture );
    ~Mesh();

    Mesh( const Mesh & ) = delete;
    Mesh &operator =( const Mesh & ) = delete;

    Mesh( Mesh &&other ) : _VBO( other._VBO ), _VAO( other._VAO ), _EBO( other._EBO )
    {
        other._VBO = 0;
        other._VAO = 0;
        other._EBO = 0;
    }

    void Render( Shader *shader );

    float *verts;
    unsigned long long verts_len;

    unsigned int *inds;
    unsigned long long inds_len;

    Texture *texture;

private:
    unsigned int _VBO;
    unsigned int _VAO;
    unsigned int _EBO;
};

#endif