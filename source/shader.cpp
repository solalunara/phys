#include "shader.h"

#include "../assets/shaders/fragment.h"
#include "../assets/shaders/vertex.h"

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

unsigned int CreateShader( ShaderType type, bool Text )
{
    unsigned int id;
    switch ( type )
    {
        case ShaderType::Vertex:
        id = glCreateShader( GL_VERTEX_SHADER );
        glShaderSource( id, 1, &g_pszVertexShaderSource, NULL );
        break;

        case ShaderType::Frag:
        if ( !Text )
        {
            id = glCreateShader( GL_FRAGMENT_SHADER );
            glShaderSource( id, 1, &g_pszFragShaderSource, NULL );
        }
        else
        {
            id = glCreateShader( GL_FRAGMENT_SHADER );
            glShaderSource( id, 1, &g_pszTextFragShaderSource, NULL );
        }
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

//shader ctor which doesn't actually create a shader so we can do other stuff first
Shader::Shader( int NullShader )
{
}

Shader::Shader( bool Text )
{
    _id = glCreateProgram();

    unsigned int vert = CreateShader( ShaderType::Vertex, Text );
    glAttachShader( id, vert );
    
    unsigned int frag = CreateShader( ShaderType::Frag, Text );
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
    Use();
    glUniform1i( glGetUniformLocation( id, name ), val );
}
void Shader::SetShaderValue( const char *name, int val )
{
    Use();
    glUniform1i( glGetUniformLocation( id, name ), val );
}
void Shader::SetShaderValue( const char *name, float val )
{
    Use();
    glUniform1f( glGetUniformLocation( id, name ), val );
}
void Shader::SetShaderValue( const char *name, glm::mat4 val )
{
    Use();
    glUniformMatrix4fv( glGetUniformLocation( id, name ), 1, false, &val[ 0 ][ 0 ] );
}