#pragma once

#include "../util/Util.h"
#include "../core/Object.h"
#include "../logging/Logger.h"
#include "../renderer/OpenGLEngineBackend.h"
#include "../renderer/AuxiliaryStructsBuilder.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <memory>
#include <string>

struct Character {
    GLuint texture;
    GLuint advance;
    glm::ivec2 size;
    glm::ivec2 bearing;
};

class Typer : public Object {
public:
    Typer(int windowWidth, int windowHeight, std::string font)
        :
        _width(static_cast<float>(windowWidth)),
        _height(static_cast<float>(windowHeight)),
        _font(std::move(font)) {
        _vertices.reserve(6 * 4);
        initFT();
    }

    ~Typer() override {
        //_engineBackend.release();
        delete _face;
    }

    void init() override {
        aux::DataBufferBuilder dataBufferBuilder;
        aux::AttributeBuilder attributeBuilder;

        auto dataBuffer = dataBufferBuilder
            .setTarget(aux::Target::vbo)
            .setSize(aux::capacity(_vertices))
            .setData(nullptr)
            .setUsage(aux::Usage::dynamic_draw)
            .build();
        _engineBackend.setDataBuffer(dataBuffer);

        auto attribute = attributeBuilder
            .setIndex(aux::Index::position)
            .setSize(4)
            .setPointer(nullptr)
            .build();
        _engineBackend.setAttribute(attribute);

        _engineBackend.setEnable(aux::Index::position);
        _engineBackend.setDrawCount(6);
    }

    void render() override {
        _shader.use();

        bind();

        float xTemp{ _x };

        for (char c : _text) {
            _currCharacter = _characters[c];

            float w = _currCharacter.size.x * _scale;
            float h = _currCharacter.size.y * _scale;
            float xpos = xTemp + _currCharacter.bearing.x * _scale;
            float ypos = _y - (_currCharacter.size.y - _currCharacter.bearing.y) * _scale;
            xTemp += (_currCharacter.advance >> 6) * _scale;

            _vertices = {
                xpos, ypos + h, 0.0f, 0.0f,
                xpos, ypos, 0.0f, 1.0f,
                xpos + w, ypos, 1.0f, 1.0f,

                xpos, ypos + h, 0.0f, 0.0f,
                xpos + w, ypos, 1.0f, 1.0f,
                xpos + w, ypos + h, 1.0f, 0.0f,
            };

            glBindTexture(GL_TEXTURE_2D, _currCharacter.texture);

            _engineBackend.bindVBO();
            aux::SubDataBufferBuilder subDataBufferBuilder;
            auto subDataBuffer = subDataBufferBuilder
                .setTarget(aux::Target::vbo)
                .setOffset(0)
                .setSize(aux::size(_vertices))
                .setData(aux::data(_vertices))
                .build();
            _engineBackend.setSubDataBuffer(subDataBuffer);
            _engineBackend.draw();
        }

        unbind();

        _shader.disuse();
    }

    void update() override {}

    void setText(std::string text) {
        _text = std::move(text);
    }

    void setScale(float scale) {
        _scale = scale;
    }

    void setColor(glm::vec4 color) {
        _color = std::move(color);
        _shaderManager.setColor(_color);
    }

    void setPosition(float x, float y, float z = 1.0f) {
        _x = x;
        _y = y;
        _z = z;
    }

    void clear() {
        _text.clear();
        _scale = 1.0f;
        _x = 0, _y = 0, _z = 0;
        setColor({ 1.0f, 1.0f, 1.0f, 1.0f });
    }

    Shader& getShader() {
        return _shader;
    }

    ShaderManager& getShaderManager() {
        return _shaderManager;
    }

    OpenGLEngineBackend& getDrawer() {
        return _engineBackend;
    }

private:
    void initFT() {
        if (_font.empty()) {
            LOGGER(error, "freetype needs a valid font");
            return;
        }

        if (_library) {
            LOGGER(warning, "freetype library is already initialized");
            return;
        }

        if (FT_Init_FreeType(&_library)) {
            LOGGER(error, "freetype library init error");
            return;
        }

        if (FT_New_Face(_library, _font.c_str(), 0, &_face)) {
            LOGGER(error, "freetype new face error");
            return;
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

    void bind() {
        glActiveTexture(GL_TEXTURE0);
    }

    void unbind() {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    float _width;
    float _height;
    std::string _text;
    std::string _font;
    float _scale{ 1.0f };
    float _x{ 0 }, _y{ 0 }, _z{ 0 };
    glm::vec4 _color{ 1.0f, 1.0f, 1.0f, 1.0f };

    Shader _shader;
    ShaderManager _shaderManager{ _shader };
    OpenGLEngineBackend _engineBackend{ _shaderManager };

    glm::mat4 _projection{};
    std::vector<float> _vertices{};

    FT_Face _face = nullptr;
    FT_Library _library = nullptr;

    Character _currCharacter{};
    std::map<GLubyte, Character> _characters{};

};
