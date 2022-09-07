#ifndef MESH_H
#define MESH_H

#pragma once

#include "transform.h"
using glm::vec2;
using glm::vec3;

struct Texture;
struct Shader;
struct Window;
struct Entity;
struct GlobalTexture;
struct Word;

struct Mesh
{
    Mesh( vec2 mins, vec2 maxs, Texture *texture, Transform transform, Window *container, bool PartOfEntity );
    ~Mesh();

    Mesh( const Mesh & ) = delete;
    Mesh &operator =( const Mesh & ) = delete;

    Mesh( Mesh &&other ) : _VBO( other._VBO ), _VAO( other._VAO ), _EBO( other._EBO ), transform( std::move( other.transform ) )
    {
        other._VBO = 0;
        other._VAO = 0;
        other._EBO = 0;
    }

    void Render();

    virtual inline bool IsText() { return false; }

    Window *container;
    Texture *texture;
    Transform transform;


    Entity *LinkedEnt = NULL;
    int LinkedEntIndex = -1;

private:
    Mesh( float *verts, unsigned long long verts_len, unsigned int *inds, unsigned long long inds_len, Texture *texture, Transform transform, Window *container, bool PartOfEntity );

    unsigned int _VBO;
    unsigned int _VAO;
    unsigned int _EBO;

    float *verts;
    unsigned long long verts_len;

    unsigned int *inds;
    unsigned long long inds_len;
};

struct Text : 
    public Mesh
{
    Text( vec2 mins, vec2 maxs, Texture *texture, Transform transform, Window *container, vec3 Color, char Character, bool UI, Word *OwningWord ) :
        Mesh( mins, maxs, texture, std::move( transform ), container, false ), Color( Color ), Character( Character ), OwningWord( OwningWord ), UI( UI )
    {
        //textures are naturally upside-down, flip them on the local x axis
        this->transform.rot = glm::angleAxis( glm::radians( 180.f ), this->transform.LocalToWorldDirection( vec3( 1, 0, 0 ) ) ) * transform.rot;
    }

    virtual bool IsText() { return true; }

    Word *OwningWord;
    vec3 Color;
    char Character;
    bool UI;
};

#endif