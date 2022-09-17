#pragma once

#include "Entity.h"
#include "Util.h"
#include "Logger.h"

#include <ft2build.h>
#include FT_FREETYPE_H

struct Character {
    GLuint texture;
    GLuint advance;
    glm::ivec2 size;
    glm::ivec2 bearing;
};

class Typer : public Entity {
public:
    Typer(ICamera* camera, int width, int height) : Entity(camera), _width(static_cast<float>(width)), _height(static_cast<float>(height)) {
        Typer::init();
        initFT();
        //Typer::initShader();
        initProjection();
        setColor();

        LOGGER(info, "typer init succeeded");
    }

    void init() override {
        glGenVertexArrays(1, &_VAO);
        glGenBuffers(1, &_VBO);

        glBindVertexArray(_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, _VBO);

        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void initFT() {
        if (_library) {
            return;
        }

        if (FT_Init_FreeType(&_library)) {
            LOGGER(error, "freetype library init error");
        }

        if (FT_New_Face(_library, _font.c_str(), 0, &_face)) {
            LOGGER(error, "freetype new face error");
        }

        FT_Set_Pixel_Sizes(_face, 0, 48 * 2);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        for (GLubyte c = 0; c < 128; c++) {
            if (FT_Load_Char(_face, c, FT_LOAD_RENDER)) {
                LOGGER(error, "freetype load char " << static_cast<char>(c) << " error");
                continue;
            }

            GLuint texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                _face->glyph->bitmap.width,
                _face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                _face->glyph->bitmap.buffer
            );
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            Character character{
                texture,
                static_cast<GLuint>(_face->glyph->advance.x),
                glm::ivec2(_face->glyph->bitmap.width, _face->glyph->bitmap.rows),
                glm::ivec2(_face->glyph->bitmap_left, _face->glyph->bitmap_top),
            };
            _characters.insert(std::pair<GLubyte, Character>(c, character));
        }

        glBindTexture(GL_TEXTURE_2D, 0);

        FT_Done_Face(_face);
        FT_Done_FreeType(_library);
        _face = nullptr;
        _library = nullptr;
    }

    void initShader(const std::string& vsCodePath, const std::string& fsCodePath) override {
        _shader.init(vsCodePath, fsCodePath);
    }

    void setFont(const std::string& font) {
        _font = font;
    }

    void setColor(glm::vec3 color = { 1.0f, 1.0f, 1.0f }) {
        _shader.use();
        _shader.setVec3("u_textColor", color);
        _shader.disuse();
    }

    void render() override {
        glBindTexture(GL_TEXTURE_2D, _currCharacter.texture);
        glBindBuffer(GL_ARRAY_BUFFER, _VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, _vertices.size() * sizeof(GLfloat), _vertices.data());
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    void renderText(std::string& text, float x, float y, float scale = 1.0f) {
        _shader.use();

        bind();

        for (char c : text) {
            _currCharacter = _characters[c];

            float xpos = x + _currCharacter.bearing.x * scale;
            float ypos = y - (_currCharacter.size.y - _currCharacter.bearing.y) * scale;

            float w = _currCharacter.size.x * scale;
            float h = _currCharacter.size.y * scale;

            _vertices = {
                xpos, ypos + h, 0.0f, 0.0f,
                xpos, ypos, 0.0f, 1.0f,
                xpos + w, ypos, 1.0f, 1.0f,

                xpos, ypos + h, 0.0f, 0.0f,
                xpos + w, ypos, 1.0f, 1.0f,
                xpos + w, ypos + h, 1.0f, 0.0f,
            };

            x += (_currCharacter.advance >> 6) * scale;

            render();
        }

        unbind();

        _shader.disuse();
    }

private:
    void initProjection() {
        _shader.use();
        _projection = glm::ortho(0.0f, (float)_width, 0.0f, (float)_height);
        _shader.setMat4("u_projection", _projection);
        _shader.disuse();
    }

    void bind() {
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(_VAO);
    }

    void unbind() {
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    float _width;
    float _height;
    glm::mat4 _projection{};
    std::vector<float> _vertices{};
    FT_Face _face = nullptr;
    FT_Library _library = nullptr;
    Character _currCharacter{};
    std::map<GLubyte, Character> _characters{};
    std::string _font = "assets/fonts/En Bloc.ttf"; // DungeonFont.ttf

};
