#ifndef WINDOW_H
#define WINDOW_H

#pragma once

#include <vector>
using std::vector;

#include "shader.h"
#include "transform.h"

struct GLFWmonitor;
struct GLFWwindow;
struct Mesh;
struct Entity;
struct Texture;

enum class WindowState : unsigned char
{
    Windowed = 0,
    Borderless = 1,
    Fullscreen = 2,
};

class Window
{
public:
    Window( WindowState state, int xres, int yres, const char *name );
    ~Window();

    Window( const Window & ) = delete;
    Window &operator =( const Window & ) = delete;
    Window( Window &&other ) :
        ID( other.ID ), shader( std::move( other.shader ) ), CameraTransform( std::move( other.CameraTransform ) )
    {
        other.ID = 0;
    }

    void SetState( WindowState state, int xres, int yres );
    void Render();

    long KeyFlags[ 6 ] { 0 };
    void SetKeyFlag( int key, bool set );
    bool GetKeyFlag( int key );

    bool LockCursor = false;

    GLFWwindow *ID;
    WindowState CurrentState;
    Shader shader;
    Transform CameraTransform;
    vector<Texture *> Textures;
    vector<Mesh *> Meshes;
    vector<Entity *> Entities;
};
inline vector<Window *> Windows = vector<Window *>();

inline Window *GetWindowFromID( GLFWwindow *ID )
{
    for ( int i = 0; i < Windows.size(); ++i )
        if ( Windows[ i ]->ID == ID ) return Windows[ i ];
    return NULL;
}

void ResizeCallback( GLFWwindow *window, int width, int height );
void KeyCallback( GLFWwindow *window, int key, int scancode, int action, int mods );

#endif