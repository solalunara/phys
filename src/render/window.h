#ifndef WINDOW_H
#define WINDOW_H

#pragma once

#include <vector>
#include <map>
#include <cstring>
using std::vector;
using std::map;

#include "shader.h"
#include "transform.h"
#include "GlobalTexture.h"

struct GLFWmonitor;
struct GLFWwindow;
struct Mesh;
struct Texture;
struct Element;
struct PhysicsBaseObject;
struct Renderable;

enum class WindowState : unsigned char
{
    Windowed = 0,
    Borderless = 1,
    Fullscreen = 2,
};

struct Window;
inline vector<Window *> Windows = vector<Window *>();

class Window
{
public:
    Window( WindowState state, float FOV, int xres, int yres, const char *name );
    ~Window();

    Window( const Window & ) = delete;
    Window &operator =( const Window & ) = delete;
    Window( Window && ) = delete;

    void SetState( WindowState state, int xres, int yres );
    void Render();

    void SetKeyFlag( int key, bool set );
    bool GetKeyFlag( int key );

    bool LockCursor = false;

    char *name;

    Shader shader;
    GLFWwindow *const &ID = _ID;

    Transform CameraTransform;
    Transform UICameraTransform;
    mat4 Perspective;
    mat4 UIPerspective;
    float FOV;

    PhysicsBaseObject *PlayerPhysics = NULL;

    vector<Texture *> Textures = vector<Texture *>();
    vector<Renderable *> Elements = vector<Renderable *>();

    static Window *GetWindowFromID( GLFWwindow *ID )
    {
        for ( int i = 0; i < Windows.size(); ++i )
            if ( Windows[ i ]->ID == ID ) return Windows[ i ];
        return NULL;
    }

private:
    GLFWwindow *_ID;
    WindowState CurrentState;
    long KeyFlags[ 6 ] { 0 };
};

void ResizeCallback( GLFWwindow *window, int width, int height );
void KeyCallback( GLFWwindow *window, int key, int scancode, int action, int mods );

#endif