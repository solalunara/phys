#ifndef ELEMENT_H
#define ELEMENT_H

#pragma once

#include "transform.h"
#include "window.h"
#include <vector>
#include <glm/glm.hpp>
using std::vector;
using glm::vec3;

struct Mesh;
struct Element;
struct PhysicsObject;
struct Collide;

struct Element
{
    Element( const Element & ) = delete;
    Element &operator =( const Element & ) = delete;
    Element( Element && ) = delete;
    virtual ~Element();

    void AddElement( Element *e );
    void RemoveElement( Element *e );
    virtual void Render();
    virtual inline bool IsMesh() { return false; }
    vector<vec3> GetNormals();
    vector<vec3> GetVertices();

    Window *const container;
    Transform *transform;
    PhysicsObject *phys_obj = NULL;
    Collide *collide = NULL;

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
