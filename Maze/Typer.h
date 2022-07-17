#pragma once

#include <iostream>
#include <map>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "ft2build.h"
#include FT_FREETYPE_H

#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "shaders/Shader.h"

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
