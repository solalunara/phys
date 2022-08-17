#ifndef WINDOW_H
#define WINDOW_H

#pragma once

struct GLFWmonitor;
struct GLFWwindow;

enum class WindowState : unsigned char
{
    Windowed = 0,
    Borderless = 1,
    Fullscreen = 2,
};

class Window
{
public:
    Window( WindowState state, int xres, int yres, const char *name, GLFWmonitor *monitor, GLFWwindow *share );

    void SetState( WindowState state );

    GLFWwindow *ID;
};

#endif