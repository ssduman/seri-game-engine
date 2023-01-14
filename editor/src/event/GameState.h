#pragma once

enum class GameState {
    idle,
    menu,
    game,
    game_over,
};

inline const char* toString(GameState gameState) {
    switch (gameState) {
        case GameState::idle: return "idle";
        case GameState::menu: return "menu";
        case GameState::game: return "game";
        case GameState::game_over: return "game_over";
        default: return "";
    }
}
