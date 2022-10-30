#pragma once

#include <iostream>

enum class GameState {
    idle,
    menu,
    game,
    gameover,
};

inline std::string to_string(GameState gameState) {
    switch (gameState) {
        case GameState::idle:
            return "idle";
        case GameState::menu:
            return "menu";
        case GameState::game:
            return "game";
        case GameState::gameover:
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
    GameState _gameState = GameState::idle;

};
