#pragma once

#include "../engine/Typer.h"
#include "../engine/Entity.h"

#include "Camera.h"
#include "Control.h"
#include "Stopwatch.h"

class Game : public Entity {
public:
    Game(ICamera* camera, Control& control, float width, float height) : Entity(camera), _control(control), _width(width), _height(height) {
        _cameraMaze = dynamic_cast<Camera*>(_camera);

        LOGGER(info, "game init succeeded");
    }

    ~Game() override {
        delete _typer;
        delete _stopwatch;

        LOGGER(info, "game delete succeeded");
    }

    void initTyper() {
        _typer = new Typer(_camera, static_cast<int>(_width), static_cast<int>(_height));
        _typer->initShader("maze-assets/shaders/typer_vs.shader", "maze-assets/shaders/typer_fs.shader", false);
        _typer->initFT("maze-assets/fonts/En Bloc.ttf");
        _typer->initProjection();
        _typer->init();
    }

    void initStopwatch() {
        _stopwatch = new Stopwatch(_typer, _width, _height);
    }

    void setProjection() {
        _shader.use();
        _projection = glm::ortho(0.0f, _width, 0.0f, _height);
        _shader.setMat4("u_projection", _projection);
    }

    void setDefaultPositions() {
        const float x = _width - _width / 6.0f;
        const float y = _height - _height / (6.0f * _width / _height);
        const float z = _height / (6.0f * _width / _height);
        const float w = _width / 6.0f;

        std::vector<glm::vec4> positions{
            { x, z, 1.0f, 1.0f }, // top right
            { w, z, 0.0f, 1.0f }, // top left
            { w, y, 0.0f, 0.0f }, // bottom left

            { x, z, 1.0f, 1.0f }, // top right
            { x, y, 1.0f, 0.0f }, // bottom right
            { w, y, 0.0f, 0.0f }, // bottom left
        };
        setDataBuffer(aux::Index::position, positions);
    }

    void render() override {
        _shader.use();
        _texture.bind();

        glBindVertexArray(_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, _VBO);
        glDrawArrays(GL_TRIANGLES, 0, _drawArrayCount);

        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void display() override {
        if (_cameraMaze->getIsPlaying()) {
            _stopwatch->run(_cameraMaze->isRestartTriggered(), _cameraMaze->checkWin());
            return;
        }

        render();

        const float x = _width / 3.0f + 45.0f;
        const float y = _height / 2.0f - 45.0f;
        _stopwatch->renderText(_control.getUserInput(), x + 6.0f * 60.0f, y, glm::vec3{ 0.3f, 0.8f, 0.9f }, 0.5f);
        _stopwatch->renderText("Enter maze size: __________", x, y, glm::vec3{ 0.5f, 0.8f, 0.2f }, 0.5f);
    }

private:
    Typer* _typer = nullptr;
    Control& _control;
    Stopwatch* _stopwatch = nullptr;
    Camera* _cameraMaze = nullptr;
    float _width{ 0.0f };
    float _height{ 0.0f };
    glm::mat4 _projection{};

};
