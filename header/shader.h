#ifndef SHADER_H
#define SHADER_H

#pragma once

#include <glm/glm.hpp>

enum class ShaderType : bool { Vertex, Frag };

unsigned int CreateShader( ShaderType type, bool Text );


struct Shader
{
    Shader( bool Text );
    Shader( int NullShader );
    ~Shader();

    Shader( const Shader & ) = delete;
    Shader &operator =( const Shader & ) = delete;

    void operator =( Shader &&other )
    {
        _id = other._id;
        other._id = 0;
    }

    Shader( Shader &&other ) : _id( other._id )
    {
        other._id = 0;
    }

    void SetShaderValue( const char *name, bool val );
    void SetShaderValue( const char *name, int val );
    void SetShaderValue( const char *name, float val );
    void SetShaderValue( const char *name, glm::mat4 val );

    void Use();

    const unsigned int &id = _id;

private:
    unsigned int _id;
};

#endif