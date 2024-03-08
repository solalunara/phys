#ifndef RENDERABLE_H
#define RENDERABLE_H

struct Window;
struct Mesh;
struct Transform;

#include <vector>
using std::vector;

#include <glm/glm.hpp>
using glm::vec3;

#include <math.h>

struct Renderable
{
    Renderable( const Renderable & ) = delete;
    Renderable &operator =( const Renderable & ) = delete;
    Renderable( Renderable && ) = delete;
    virtual ~Renderable();

    void AddChild( Renderable *e );
    void RemoveChild( Renderable *e );
    virtual void Render();
    virtual inline bool IsMesh() { return false; }
    vector<vec3> GetNormals();
    vector<float> GetPlaneDists();
    vector<Mesh *> GetMeshes();
    vector<vec3> GetVertices( bool local = false );
    float SmallestInterPointDist();

    Window *const container;
    Transform *transform;

protected:
    Renderable( Window *container, Transform *transform, vector<Renderable *> Elements );
    vector<Renderable *> Elements;

};

#endif // RENDERABLE_H