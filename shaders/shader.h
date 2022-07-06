#ifndef SHADER_H
#define SHADER_H

#pragma once

#include <glm/glm.hpp>

enum class Shader : bool { Vertex, Frag };

unsigned int CreateShader( Shader type );
unsigned int CreateShaderProgram();
void SetShaderValue( unsigned int id, const char *name, bool val );
void SetShaderValue( unsigned int id, const char *name, int val );
void SetShaderValue( unsigned int id, const char *name, float val );
void SetShaderValue( unsigned int id, const char *name, glm::mat4 val );

#endif