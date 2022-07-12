#include "shader.h"

#include "source/fragment.h"
#include "source/vertex.h"

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

unsigned int CreateShader( ShaderType type )
{
    unsigned int id;
    switch ( type )
    {
        case ShaderType::Vertex:
        id = glCreateShader( GL_VERTEX_SHADER );
        glShaderSource( id, 1, &g_pszVertexShaderSource, NULL );
        break;

        case ShaderType::Frag:
        id = glCreateShader( GL_FRAGMENT_SHADER );
        glShaderSource( id, 1, &g_pszFragShaderSource, NULL );
        break;
    }
    glCompileShader( id );

    int state;
    glGetShaderiv( id, GL_COMPILE_STATUS, &state );
    if ( !state )
    {
        char info[ 2048 ] { 0 };
        glGetShaderInfoLog( id, 2048, NULL, info );
        const char *name = type == ShaderType::Frag ? "fragment" : "vertex";
        printf( "Couldn't compile %s shader - %s\n", name, info );
    }

    return id;
}

Shader::Shader()
{
    _id = glCreateProgram();

    unsigned int vert = CreateShader( ShaderType::Vertex );
    glAttachShader( id, vert );
    
    unsigned int frag = CreateShader( ShaderType::Frag );
    glAttachShader( id, frag );

    glLinkProgram( id );

    int state;
    glGetProgramiv( id, GL_LINK_STATUS, &state );
    if ( !state )
    {
        char info[ 2048 ] { 0 };
        glGetProgramInfoLog( id, 2048, NULL, info );
        printf( "Couldn't link shader program - %s\n", info );
    }

    glDeleteShader( vert );
    glDeleteShader( frag );
}
Shader::~Shader()
{
    glDeleteProgram( id );
    _id = 0;
}

void Shader::Use()
{
    glUseProgram( id );
}

void Shader::SetShaderValue( const char *name, bool val )
{
    glUniform1i( glGetUniformLocation( id, name ), val );
}
void Shader::SetShaderValue( const char *name, int val )
{
    glUniform1i( glGetUniformLocation( id, name ), val );
}
void Shader::SetShaderValue( const char *name, float val )
{
    glUniform1f( glGetUniformLocation( id, name ), val );
}
void Shader::SetShaderValue( const char *name, glm::mat4 val )
{
    glUniformMatrix4fv( glGetUniformLocation( id, name ), 1, false, &val[ 0 ][ 0 ] );
}