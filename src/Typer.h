#pragma once

#include "Shader.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <map>
#include <iostream>
#include <string>

struct Character {
    unsigned int TextureID;
    glm::ivec2 Size;
    glm::ivec2 Bearing;
    unsigned int Advance;
};

class Typer {
private:
    Shader* shader;
    glm::mat4 projection;
    float width, height;
    std::map<char, Character> Characters;
    unsigned int VAO, VBO;
    FT_Library ft;
    FT_Face face;
public:
    Typer(int w, int h);
    void renderText(std::string text, float x, float y, float scale, glm::vec3 color);
};
