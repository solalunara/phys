#ifndef ELEMENT_H
#define ELEMENT_H

#pragma once

#include "transform.h"
#include "window.h"
#include <vector>
using std::vector;

struct Mesh;
struct Element;

struct Element
{
    Element( const Element & ) = delete;
    Element &operator =( const Element & ) = delete;
    Element( Element && ) = delete;
    ~Element();

    void AddElement( Element *e );
    void RemoveElement( Element *e );
    virtual void Render();
    virtual inline bool IsMesh() { return false; }

    Window *const container;
    Transform *transform;

protected:
    Element( Window *container, Transform *transform, vector<Element *> Elements );
    vector<Element *> Elements;
};

struct UIElement :
    public Element
{
    UIElement( Window *container, Transform *transform ) :
        Element( container, transform, vector<Element *>() )
    {
    }

    virtual void Render();
};

struct GameElement :
    public Element
{
    GameElement( Window *container, Transform *transform ) :
        Element( container, transform, vector<Element *>() )
    {
    }

    virtual void Render();
};

#endif
