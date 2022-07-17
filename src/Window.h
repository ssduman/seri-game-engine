#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

class Window {
private:
    int width = 0;
    int height = 0;
    double mouseX = 0, mouseY = 0;
    GLFWwindow* window;
public:
    Window(bool fullscreen);
    int getWidth();
    int getHeight();
    double getMouseX();
    double getMouseY();
    void setCursorPosMiddle();
    GLFWwindow* getWindow();
};

