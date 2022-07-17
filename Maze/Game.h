#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <ctime>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "stb_image/stb_image.h"

#include "shaders/Shader.h"
#include "Typer.h"

class Game {
private:
    Shader* shader;
    Typer* typer;
    std::clock_t start = NULL;
    int sec = 0, min = 0, hour = 0;
    float timer = 0.0f;
    std::string h_m_s = "";
    glm::mat4 projection;
    int width, height;
    bool inExit = false;
    unsigned int texture;
    unsigned int VBO, VAO;
    float vertices[24] = {
        1460.0f,   15.0f, 1.0f, 1.0f, // top right
         460.0f,   15.0f, 0.0f, 1.0f, // top left
         460.0f, 1015.0f, 0.0f, 0.0f, // bottom left

        1460.0f,   15.0f, 1.0f, 1.0f, // top right
        1460.0f, 1015.0f, 1.0f, 0.0f, // bottom right
         460.0f, 1015.0f, 0.0f, 0.0f, // bottom left
    };
    void stopwatch(bool& restart);
public:
    Game(float w, float h);
    void display(std::string userInput, bool play, bool& restart, bool exit);
    void renderText(std::string text, float x, float y, float scale, glm::vec3 color) {
        typer->renderText(text, x, y, scale, color);
    }
};