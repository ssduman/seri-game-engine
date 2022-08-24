#pragma once

#include <iostream>

enum class GameState {
    IDLE,
    MENU,
    GAME,
    GAMEOVER,
};

std::string to_string(GameState gameState) {
    switch (gameState) {
        case GameState::IDLE:
            return "idle";
        case GameState::MENU:
            return "menu";
        case GameState::GAME:
            return "game";
        case GameState::GAMEOVER:
            return "game over";
        default:
            return "unknown";
    }
}

class State {
public:
    GameState& gameState() {
        return _gameState;
    }

private:
    GameState _gameState = GameState::IDLE;

};
