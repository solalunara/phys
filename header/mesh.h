#ifndef MESH_H
#define MESH_H

#pragma once

#include "transform.h"
#include "element.h"
using glm::vec2;
using glm::vec3;

struct Texture;
struct Shader;
struct Window;
struct Entity;
struct GlobalTexture;
struct Word;

struct Mesh :
    public Element
{
    Mesh( vec2 mins, vec2 maxs, Texture *texture, Transform *transform, Window *container );
    ~Mesh();

    Mesh( const Mesh & ) = delete;
    Mesh &operator =( const Mesh & ) = delete;

    Mesh( Mesh &&other ) = delete;

    void Render();

    virtual inline bool IsText() { return false; }
    virtual inline bool IsMesh() { return true; }

    Texture *const &texture = _texture;

protected:
    Mesh( float *verts, unsigned long long verts_len, unsigned int *inds, unsigned long long inds_len, Texture *texture, Transform *transform, Window *container );

    unsigned int _VBO;
    unsigned int _VAO;
    unsigned int _EBO;

    float *verts;
    unsigned long long verts_len;

    unsigned int *inds;
    unsigned long long inds_len;

    Texture *_texture;
};

struct CharacterMesh : 
    public Mesh
{
    CharacterMesh( vec2 mins, vec2 maxs, Texture *texture, Transform *transform, Window *container, vec3 Color, char c, bool UI ) :
        Mesh( mins, maxs, texture, transform, container ), Color( Color ), c( c )
    {
        //textures are naturally upside-down, flip them on the local x axis
        this->transform->rot = glm::angleAxis( glm::radians( 180.f ), this->transform->LocalToWorldDirection( vec3( 1, 0, 0 ) ) ) * transform->rot;
    }

    virtual bool IsText() { return true; }

    vec3 Color;
    char c;
};

#endif