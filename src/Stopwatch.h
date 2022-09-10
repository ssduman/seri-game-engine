#pragma once

#include "Util.h"
#include "Typer.h"

#include <ctime>

class Stopwatch {
public:
    Stopwatch(Typer* typer, float width, float height) : _typer(typer), _width(width), _height(height) {}

    void init() {
        _startedClockValue = std::clock();
    }

    void run(bool& isRestartTriggered, const bool isWon) {
        if (_startedClockValue == 0L) {
            init();
        }

        if (isRestartTriggered) {
            _sec = 0;
            _min = 0;
            _hour = 0;
            _gameFinished = false;
            _startedClockValue = 0L;
            isRestartTriggered = false;
        }

        if (isWon) {
            _gameFinished = true;
        }

        checkWonStatus(isWon);

        calculateStopwatch(isWon);
    }

    void renderText(std::string text, float x, float y, glm::vec3 color = { 1.0f, 1.0f, 1.0f }, float scale = 1.0f) {
        _typer->setColor(color);
        _typer->renderText(text, x, y, scale);
    }

private:
    void checkWonStatus(const bool isWon) {
        if (isWon || _gameFinished) {
            static float _blinkTimer = 0.0f;
            _blinkTimer += 0.01f;
            if (_blinkTimer >= 0.16f) {
                if (_blinkTimer >= 0.32f) {
                    _blinkTimer = 0.0f;
                }

                renderText(_hms, _width - _width / 5.0f, _height - _height / 12.0f, glm::vec3{ 0.95f, 0.6f, 0.0f });
                renderText("YOU WON!", _width / 2.8f, _height / 2.0f, glm::vec3{ 0.95f, 0.5f, 1.0f });
            }
        }
    }

    void calculateStopwatch(const bool isWon) {
        if (!isWon && !_gameFinished) {
            _sec = floor((std::clock() - _startedClockValue) / static_cast<double>(CLOCKS_PER_SEC));
            if (_sec == 60) {
                _startedClockValue = 0L;
                _sec = 0;
                _min++;
                if (_min == 60) {
                    _min = 0;
                    _hour++;
                }
            }

            std::string sec_s = _sec < 10 ? "0" + std::to_string(_sec) : std::to_string(_sec);
            std::string min_s = _min < 10 ? "0" + std::to_string(_min) : std::to_string(_min);
            std::string hour_s = _hour < 10 ? "0" + std::to_string(_hour) : std::to_string(_hour);
            _hms = hour_s + ":" + min_s + ":" + sec_s;
        }

        renderText(_hms, _width - _width / 5.0f, _height - _height / 12.0f, glm::vec3{ 0.95f, 0.6f, 0.0f });
    }

    Typer* _typer = nullptr;
    float _width{ 0.0f };
    float _height{ 0.0f };
    bool _gameFinished{ false };

    int _sec{ 0 };
    int _min{ 0 };
    int _hour{ 0 };
    std::string _hms{};
    std::clock_t _startedClockValue{ 0L };

};