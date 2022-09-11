#ifndef ELEMENT_H
#define ELEMENT_H

#pragma once

#include "transform.h"
#include "window.h"
#include "mesh.h"
#include <vector>
using std::vector;

struct Mesh;

struct Element
{
    Element( const Element & ) = delete;
    Element &operator =( const Element & ) = delete;
    Element( Element && ) = delete;
    ~Element();

    virtual void Render();

    Window *const container;
    Transform transform;

protected:
    Element( Window *container, Transform &&transform, vector<Mesh *> Meshes ) :
        container( container ), Meshes( Meshes ), transform( (Transform &&)transform )
    {
        for ( int i = 0; i < Meshes.size(); ++i )
            if ( !Meshes[ i ]->transform.HasParent() ) Meshes[ i ]->transform.SetParent( &transform );
        container->Elements.push_back( this );
    }

    vector<Mesh *> Meshes;

};

struct UIElement :
    public Element
{
    UIElement( Window *container, Transform &&transform ) :
        Element( container, (Transform &&)transform, vector<Mesh *>() )
    {
    }

    virtual void Render();
};

struct GameElement :
    public Element
{
    GameElement( Window *container, Transform &&transform ) :
        Element( container, (Transform &&)transform, vector<Mesh *>() )
    {
    }

    virtual void Render();
};

#endif
