#pragma once

#include "Typer.h"
#include "Entity.h"
#include "Stopwatch.h"

#include <stb_image.h>

class Game : public Entity {
public:
    Game(ICamera* camera, float width, float height) : Entity(camera), _width(width), _height(height) {
        init();
        initShader();
        initTyper();
        initTexture();
        initStopwatch();
        setProjection();

        LOGGER(info, "game init succeeded");
    }

    virtual ~Game() {
        delete _typer;
        delete _stopwatch;
    }

    void display(std::string userInput, const bool isPlaying, bool& isRestartTriggered, const bool isWon) {
        if (isPlaying) {
            _stopwatch->run(isRestartTriggered, isWon);
            return;
        }

        render();

        float x = _width / 3.0f + 45.0f;
        float y = _height / 2.0f - 45.0f;
        _stopwatch->renderText(userInput, x + 8.0f * 46.5f, y, glm::vec3{ 0.3f, 0.8f, 0.9f }, 0.5f);
        _stopwatch->renderText("Enter maze size: __________", x, y, glm::vec3{ 0.5f, 0.8f, 0.2f }, 0.5f);
    }

private:
    void init() override {
        _vertices = {
            1460.0f, 15.0f, 1.0f, 1.0f, // top right
            460.0f, 15.0f, 0.0f, 1.0f, // top left
            460.0f, 1015.0f, 0.0f, 0.0f, // bottom left

            1460.0f, 15.0f, 1.0f, 1.0f, // top right
            1460.0f, 1015.0f, 1.0f, 0.0f, // bottom right
            460.0f, 1015.0f, 0.0f, 0.0f, // bottom left
        };

        auto w = _width, h = _height;
        _vertices[0] = _vertices[12] = _vertices[16] = w - w / 6.0f;
        _vertices[9] = _vertices[17] = _vertices[21] = h - h / (6.0f * w / h);
        _vertices[1] = _vertices[5] = _vertices[13] = h / (6.0f * w / h);
        _vertices[4] = _vertices[8] = _vertices[20] = w / 6.0f;

        glGenVertexArrays(1, &_VAO);
        glGenBuffers(1, &_VBO);

        glBindVertexArray(_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, _VBO);

        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void initShader(const std::string& vsCodePath = "shaders/game_vs.shader", const std::string& fsCodePath = "shaders/game_fs.shader") override {
        _shader.init(vsCodePath, fsCodePath);
    }

    void initTyper() {
        _typer = new Typer(_camera, static_cast<int>(_width), static_cast<int>(_height));
    }

    void initTexture() {
        glGenTextures(1, &_texture);
        glBindTexture(GL_TEXTURE_2D, _texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        int tWidth, tHeight, tComponents;
        unsigned char* image = stbi_load(_texturePath.c_str(), &tWidth, &tHeight, &tComponents, 0);
        if (image) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tWidth, tHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
            stbi_image_free(image);
        } else {
            LOGGER(error, "texture " << _texturePath << " could not loaded");
        }

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void initStopwatch() {
        _stopwatch = new Stopwatch(_typer, _width, _height);
    }

    void setProjection() {
        _shader.use();
        auto projection = glm::ortho(0.0f, _width, 0.0f, _height);
        _shader.setMat4("u_projection", projection);
    }

    void update() override {}

    void render() override {
        _shader.use();

        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(_VAO);
        glBindTexture(GL_TEXTURE_2D, _texture);
        glBindBuffer(GL_ARRAY_BUFFER, _VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, _vertices.size() * sizeof(GLfloat), _vertices.data());
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    Typer* _typer = nullptr;
    Stopwatch* _stopwatch = nullptr;;
    float _width{ 0.0f };
    float _height{ 0.0f };
    unsigned int _texture{ 0 };
    std::string _texturePath{ "textures/gameWindow.png" };

};