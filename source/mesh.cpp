#include "mesh.h"

#include "shader.h"
#include "texture.h"
#include "shader.h"
#include "window.h"
#include "GlobalTexture.h"
#include "cube.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <string.h>
#include <string>
#include <stdexcept>


Mesh::Mesh( float *verts, unsigned long long verts_len, unsigned int *inds, unsigned long long inds_len, Texture *texture, Transform *transform, Window *container ) : 
    verts_len( verts_len ), inds_len( inds_len ), _texture( texture ), Element( container, transform, vector<Element *>() )
{
    if ( !container )
        throw std::invalid_argument( "Attempted to make mesh with no container!\n" );

    glfwMakeContextCurrent( container->ID );
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

    __verts_pts.reserve( verts_len / 5 );
    for ( int i = 0; i < verts_len / 5; ++i )
        __verts_pts.push_back( vec3( verts[ i*5 + 0 ], verts[ i*5 + 1 ], verts[ i*5 + 2 ] ) );
    __verts_pts.shrink_to_fit();


    __smallest_inter_point_dist = INFINITY;
    for ( int i = 0; i < verts_len / 5; ++i )
        for ( int j = 0; j < verts_len / 5; ++j )
            if ( i != j && glm::length( __verts_pts[ i ] - __verts_pts[ j ] ) < __smallest_inter_point_dist )
                __smallest_inter_point_dist = glm::length( __verts_pts[ i ] - __verts_pts[ j ] );


    if ( verts_len >= 3*5 )
    {
        vec3 a = __verts_pts[ 1 ] - __verts_pts[ 0 ];
        vec3 b = __verts_pts[ 2 ] - __verts_pts[ 1 ];
        _norm = glm::normalize( glm::cross( a, b ) );
        if ( verts_len > 3*5 )
        {
            float dist = glm::dot( a, _norm );
            for ( int i = 4; i < verts_len / 5; ++i )
                if ( std::abs( glm::dot( __verts_pts[ i ], _norm ) - dist ) > 1e-2f )
                    throw std::invalid_argument( "Mesh does not lie on a plane!" );
        }
    }
}

Mesh::Mesh( glm::vec2 mins, glm::vec2 maxs, Texture *texture, Transform *transform, Window *container ) :
    _texture( texture ), Element( container, transform, vector<Element *>() )
{
    this->verts = new float[] {
        maxs.x, maxs.y, .0f,        1.0f, 1.0f,
        maxs.x, mins.y, .0f,        1.0f, 0.0f,
        mins.x, mins.y, .0f,        0.0f, 0.0f,
        mins.x, maxs.y, .0f,        0.0f, 1.0f,
    };
    this->verts_len = 20;
    this->inds = new unsigned int[] {
        0, 1, 3,
        1, 2, 3
    };
    this->inds_len = 6;

    if ( !container )
        throw std::invalid_argument( "Attempted to make mesh with no container!\n" );

    glfwMakeContextCurrent( container->ID );

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

    __verts_pts.reserve( verts_len / 5 );
    for ( int i = 0; i < verts_len / 5; ++i )
        __verts_pts.push_back( vec3( verts[ i*5 + 0 ], verts[ i*5 + 1 ], verts[ i*5 + 2 ] ) );
    __verts_pts.shrink_to_fit();


    __smallest_inter_point_dist = INFINITY;
    for ( int i = 0; i < verts_len / 5; ++i )
        for ( int j = 0; j < verts_len / 5; ++j )
            if ( i != j && glm::length( __verts_pts[ i ] - __verts_pts[ j ] ) < __smallest_inter_point_dist )
                __smallest_inter_point_dist = glm::length( __verts_pts[ i ] - __verts_pts[ j ] );


    if ( verts_len >= 3*5 )
    {
        vec3 a = __verts_pts[ 1 ] - __verts_pts[ 0 ];
        vec3 b = __verts_pts[ 2 ] - __verts_pts[ 1 ];
        _norm = glm::normalize( glm::cross( a, b ) );
        if ( verts_len > 3*5 )
        {
            float dist = glm::dot( a, _norm );
            for ( int i = 4; i < verts_len / 5; ++i )
                if ( std::abs( glm::dot( __verts_pts[ i ], _norm ) - dist ) > 1e-2f )
                    throw std::invalid_argument( "Mesh does not lie on a plane!" );
        }
    }
}

Mesh::~Mesh()
{
    glDeleteVertexArrays( 1, &_VAO );
    glDeleteBuffers( 1, &_VBO );
    glDeleteBuffers( 1, &_EBO );
    _VAO = _VBO = _EBO = 0;
    delete[] verts;
    delete[] inds;
}

void Mesh::Render()
{
    container->shader.SetShaderValue( "Text", IsText() );
    container->shader.SetShaderValue( "Transform", transform->GetMatrix() );
    if ( IsText() )
    {
        CharacterMesh *t = static_cast<CharacterMesh *>( this );
        container->shader.SetShaderValue( "TextColorX", t->Color.x );
        container->shader.SetShaderValue( "TextColorY", t->Color.y );
        container->shader.SetShaderValue( "TextColorZ", t->Color.z );
    }
    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, texture->id );
    glBindVertexArray( _VAO );
    glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0 );
}

vec3 Mesh::GetNormal()
{
    vec3 norm = transform->LocalToWorldDirection( _norm );
    return glm::round( norm * 1e4f ) / 1e4f;
}

float Mesh::GetPlaneDist()
{
    return glm::dot( transform->GetAbsOrigin(), GetNormal() );
}

vector<vec3> Mesh::GetVertices()
{
    vector<vec3> result;
    result.reserve( verts_len / 5 );
    for ( int i = 0; i < __verts_pts.size(); ++i )
    {
        result.push_back( transform->LocalToWorldPoint( __verts_pts[ i ] ) );
    }
    return result;
}