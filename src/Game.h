#pragma once

#include "Typer.h"
#include "Entity.h"

#include <stb_image.h>

#include <ctime>

class Game : public Entity {
public:
    Game(float width, float height) : Entity(nullptr), _width(width), _height(height) {
        init();
        initShader();
        initTyper();
        initTexture();
        setProjection();

        LOGGER(info, "game init succeeded");
    }

    void display(std::string userInput, const bool play, const bool restart, const bool exit) {
        if ((_inExit == true) && (restart == true)) {
            _inExit = false;
        } else if (_inExit == true) {
            _inExit = true;
        } else {
            _inExit = exit;
        }
        if (play == true) {
            stopwatch(restart);
            return;
        }

        render();

        float x = _width / 3.0f + 45.0f;
        float y = _height / 2.0f - 45.0f;
        renderText(userInput, x + 8.0f * 46.5f, y, 0.5f, glm::vec3(0.3f, 0.8f, 0.9f));
        renderText("Enter maze size: __________", x, y, 0.5f, glm::vec3(0.5f, 0.8f, 0.2f));
    }

    void renderText(std::string text, float x, float y, float scale, glm::vec3 color) {
        _typer->setColor(color);
        _typer->renderText(text, x, y, scale);
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

        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_STATIC_DRAW);
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
        _typer = new Typer(static_cast<int>(_width), static_cast<int>(_height));
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

    void setProjection() {
        _shader.use();
        auto projection = glm::ortho(0.0f, _width, 0.0f, _height);
        _shader.setMat4("projection", projection);
    }

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

    void update() override {}

    void stopwatch(const bool restart) {
        if (start == NULL) start = std::clock();
        if (restart == true) {
            start = 0, hour = 0, min = 0, sec = 0;
            _inExit = false;
        }
        if (_inExit == true) {
            timer += 0.01f;
            if (timer >= 0.16f) {
                renderText(h_m_s, _width - _width / 5.0f, _height - _height / 12.0f, 1.0f, glm::vec3(0.95f, 0.6f, 0.0f));
                renderText("YOU WON!", _width / 2.8f, _height / 2.0f, 1.0f, glm::vec3(0.95f, 0.5f, 1.0f));
                if (timer >= 0.32f) timer = 0.0f;
            }
            return;
        }
        sec = floor((std::clock() - (double)start) / (double)CLOCKS_PER_SEC);
        if (sec == 60) {
            sec = 0;
            start = 0;
            min++;
            if (min == 60) {
                min = 0;
                hour++;
            }
        }
        std::string sec_s = sec < 10 ? "0" + std::to_string(sec) : std::to_string(sec);
        std::string min_s = min < 10 ? "0" + std::to_string(min) : std::to_string(min);
        std::string hour_s = hour < 10 ? "0" + std::to_string(hour) : std::to_string(hour);
        h_m_s = hour_s + ":" + min_s + ":" + sec_s;
        renderText(h_m_s, _width - _width / 5.0f, _height - _height / 12.0f, 1.0f, glm::vec3(0.95f, 0.6f, 0.0f));
    }

    Typer* _typer = nullptr;
    float _width{ 0.0f };
    float _height{ 0.0f };
    bool _inExit = false;
    unsigned int _texture{ 0 };
    std::string _texturePath{ "textures/gameWindow.png" };

    std::clock_t start{ 0L };
    int sec = 0, min = 0, hour = 0;
    float timer = 0.0f;
    std::string h_m_s = "";

};