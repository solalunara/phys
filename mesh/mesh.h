#ifndef MESH_H
#define MESH_H

#pragma once


struct Mesh
{
    Mesh( unsigned int VBO, unsigned int VAO, unsigned int EBO ) : VBO( VBO ), VAO( VAO ), EBO( EBO ) {}
    Mesh( float *verts, size_t verts_len, unsigned int *inds, size_t inds_len );
    ~Mesh();

    float *verts;
    size_t verts_len;

    unsigned int *inds;
    size_t inds_len;

    unsigned int VBO;
    unsigned int VAO;
    unsigned int EBO;
};

#endif