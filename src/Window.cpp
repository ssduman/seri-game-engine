#include "Window.h"

Window::Window(bool fullscreen, int w, int h) {
    if (!glfwInit()) {
        std::cout << "glfwInit error" << std::endl;
        exit(-1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    int monXPos, monYPos;
    int widthTemp, heightTemp;
    glfwGetMonitorWorkarea(glfwGetPrimaryMonitor(), &monXPos, &monYPos, &widthTemp, &heightTemp);
    if (fullscreen) {
        this->window = glfwCreateWindow(widthTemp, heightTemp, "Maze", glfwGetPrimaryMonitor(), NULL);
        this->width = widthTemp;
        this->height = heightTemp;
    }
    else {
        this->window = glfwCreateWindow(w, h, "Maze", NULL, NULL);
        this->width = w;
        this->height = h;
    }

    if (!window) {
        glfwTerminate();
        std::cout << "window error" << std::endl;
        exit(-1);
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK) {
        std::cout << "glewInit Error" << std::endl;
        exit(-1);
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_FRONT);
    //glFrontFace(GL_CW);
    //glFrontFace(GL_CCW);
    //glCullFace(GL_BACK);

    //setCursorPosMiddle();
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

int Window::getWidth() {
    return this->width;
}

int Window::getHeight() {
    return this->height;
}

double Window::getMouseX() {
    glfwGetCursorPos(window, &mouseX, &mouseY);
    return mouseX;
}

double Window::getMouseY() {
    glfwGetCursorPos(window, &mouseX, &mouseY);
    return mouseY;
}

void Window::setCursorPosMiddle() {
    glfwSetCursorPos(this->window, width / 2, height / 2);
}

GLFWwindow* Window::getWindow() {
    return this->window;
}