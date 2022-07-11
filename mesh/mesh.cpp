#include "mesh.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <string.h>


Mesh::Mesh( float *verts, unsigned long long verts_len, unsigned int *inds, unsigned long long inds_len, unsigned int shader, unsigned int texture ) : 
    verts_len( verts_len ), inds_len( inds_len ), shader( shader ), texture( texture )
{
    this->verts = new float[ verts_len ];
    memcpy( this->verts, verts, verts_len * sizeof( float ) );

    this->inds = new unsigned int[ inds_len ];
    memcpy( this->inds, inds, inds_len * sizeof( unsigned int ) );

    glUseProgram( shader );


    glGenVertexArrays( 1, &VAO );
    glGenBuffers( 1, &VBO );
    glGenBuffers( 1, &EBO );

    glBindVertexArray( VAO );
    
    glBindBuffer( GL_ARRAY_BUFFER, VBO );
    glBufferData( GL_ARRAY_BUFFER, sizeof( float ) * verts_len, verts, GL_DYNAMIC_DRAW );

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, EBO );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( unsigned int ) * inds_len, inds, GL_DYNAMIC_DRAW );

    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof( float ), (void *)0 );
    glEnableVertexAttribArray( 0 );  
    glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof( float ), (void *)( 3 * sizeof( float ) ) );
    glEnableVertexAttribArray( 1 );

    glBindVertexArray( 0 );
    glUseProgram( 0 );
}

Mesh::~Mesh()
{
    glDeleteVertexArrays( 1, &VAO );
    glDeleteBuffers( 1, &VBO );
    glDeleteBuffers( 1, &EBO );
}

void Mesh::Render()
{
    glUseProgram( shader );
    glBindTexture( GL_TEXTURE_2D, texture );
    glBindVertexArray( VAO );
    glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0 );
}