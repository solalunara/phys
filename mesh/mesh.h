#ifndef MESH_H
#define MESH_H

#pragma once


struct Mesh
{
    Mesh( float *verts, unsigned long long verts_len, unsigned int *inds, unsigned long long inds_len, unsigned int texture );
    ~Mesh();

    void Render( unsigned int shader );

    float *verts;
    unsigned long long verts_len;

    unsigned int *inds;
    unsigned long long inds_len;

    unsigned int shader;
    unsigned int texture;

    unsigned int VBO;
    unsigned int VAO;
    unsigned int EBO;
};

#endif