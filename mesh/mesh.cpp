#include "mesh.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <string.h>


Mesh::Mesh( float *verts, size_t verts_len, unsigned int *inds, size_t inds_len ) : 
    verts_len( verts_len ), inds_len( inds_len )
{
    this->verts = new float[ verts_len ];
    memcpy( this->verts, verts, verts_len * sizeof( float ) );

    this->inds = new unsigned int[ inds_len ];
    memcpy( this->inds, inds, inds_len * sizeof( unsigned int ) );

    glGenBuffers( 1, &EBO );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, EBO );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( unsigned int ) * inds_len, inds, GL_DYNAMIC_DRAW );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, NULL );

    glGenVertexArrays( 1, &VAO );
    glBindVertexArray( VAO );
    
    glGenBuffers( 1, &VBO );
    glBindBuffer( GL_ARRAY_BUFFER, VBO );
    glBufferData( GL_ARRAY_BUFFER, sizeof( float ) * verts_len, verts, GL_DYNAMIC_DRAW );
    glBindBuffer( GL_ARRAY_BUFFER, NULL );

    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof( float ), (void *)0 );
    glEnableVertexAttribArray( 0 );  
    glVertexAttribPointer( 1, 2, GL_UNSIGNED_INT, GL_FALSE, 5 * sizeof( unsigned int ), (void *)( 3 * sizeof( float ) ) );
    glEnableVertexAttribArray( 1 );

    glBindVertexArray( NULL );
}

Mesh::~Mesh()
{
    glDeleteBuffers( 1, &VBO );
    glDeleteVertexArrays( 1, &VAO );
    glDeleteBuffers( 1, &EBO );
}