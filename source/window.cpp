#include "window.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

Window::Window( WindowState state, int xres, int yres, const char *name, GLFWmonitor *monitor, GLFWwindow *share )
{
    ID = glfwCreateWindow( xres, yres, name, monitor, share );
    switch ( state )
    {
        case WindowState::Windowed:
        break;

        case WindowState::Borderless:
        const GLFWvidmode* mode = glfwGetVideoMode( monitor );
        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
        break;

        case WindowState::Fullscreen:

        break;
    }
}