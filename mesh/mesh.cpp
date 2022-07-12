#include "mesh.h"

#include "../shaders/shader.h"
#include "../textures/texture.h"
#include "../shaders/shader.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <string.h>


Mesh::Mesh( float *verts, unsigned long long verts_len, unsigned int *inds, unsigned long long inds_len, Texture *texture ) : 
    verts_len( verts_len ), inds_len( inds_len ), texture( texture )
{
    this->verts = new float[ verts_len ];
    memcpy( this->verts, verts, verts_len * sizeof( float ) );

    this->inds = new unsigned int[ inds_len ];
    memcpy( this->inds, inds, inds_len * sizeof( unsigned int ) );

    glGenVertexArrays( 1, &_VAO );
    glGenBuffers( 1, &_VBO );
    glGenBuffers( 1, &_EBO );

    glBindVertexArray( _VAO );
    
    glBindBuffer( GL_ARRAY_BUFFER, _VBO );
    glBufferData( GL_ARRAY_BUFFER, sizeof( float ) * verts_len, verts, GL_STATIC_DRAW );

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _EBO );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( unsigned int ) * inds_len, inds, GL_STATIC_DRAW );

    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof( float ), (void *)0 );
    glEnableVertexAttribArray( 0 );  
    glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof( float ), (void *)( 3 * sizeof( float ) ) );
    glEnableVertexAttribArray( 1 );

    glBindVertexArray( 0 );
}

Mesh::~Mesh()
{
    glDeleteVertexArrays( 1, &_VAO );
    glDeleteBuffers( 1, &_VBO );
    glDeleteBuffers( 1, &_EBO );
    _VAO = _VBO = _EBO = 0;
}

void Mesh::Render( Shader *shader )
{
    shader->Use();
    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, texture->id );
    glBindVertexArray( _VAO );
    glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0 );
}