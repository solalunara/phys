#ifndef ELEMENT_H
#define ELEMENT_H

#pragma once

#include "render/transform.h"
#include "render/window.h"
#include "render/renderable.h"
#include <vector>
#include <glm/glm.hpp>
using std::vector;
using glm::vec3;

struct Mesh;
struct Element;
struct PhysicsObject;
struct Collide;

struct Element :
    public Renderable
{
    Element( const Element & ) = delete;
    Element &operator =( const Element & ) = delete;
    Element( Element && ) = delete;
    virtual ~Element();

    virtual void Render();
    virtual inline bool IsMesh() { return false; }

    PhysicsObject *phys_obj = NULL;
    Collide *collide = NULL;

protected:
    Element( Window *container, Transform *transform, vector<Renderable *> Elements );

};

struct UIElement :
    public Element
{
    UIElement( Window *container, Transform *transform ) :
        Element( container, transform, vector<Renderable *>() )
    {
    }

    virtual void Render();
};

struct GameElement :
    public Element
{
    GameElement( Window *container, Transform *transform ) :
        Element( container, transform, vector<Renderable *>() )
    {
    }

    virtual void Render();
};

#endif
