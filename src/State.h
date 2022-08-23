#pragma once

enum class GameState {
    IDLE,
    MENU,
    GAME,
    GAMEOVER,
};

class State {
public:
    GameState& gameState() {
        return _gameState;
    }

private:
    GameState _gameState;

};
