#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <vector>
#include <iostream>

class Renderer {
private:
    unsigned int va, count = 0;
    unsigned int indices[36] = {
        0,   1,  3,  1,  2,  3, // BACK
        4,   5,  6,  4,  6,  7, // FRONT
        8,   9, 10,  8, 10, 11, // LEFT
        12, 13, 15, 13, 14, 15, // RIGHT
        17, 18, 19, 16, 19, 18, // BOTTOM
        21, 22, 23, 20, 21, 23  // TOP
    };

public:
    Renderer(float w, float h, float z);
    void init(unsigned int c, std::vector<glm::vec3>& coordinates);
    void render();
};


