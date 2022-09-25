#pragma once

struct SnakeProperties {
    SnakeProperties(float width_, float height_, float interval_)
        :
        width{ width_ },
        height{ height_ },
        interval{ interval_ },
        totalRows{ static_cast<int>(width / interval) },
        totalCols{ static_cast<int>(height / interval) },
        totalCells{ totalRows * totalCols } {}

    const float width;
    const float height;
    const float interval;

    const int totalRows;
    const int totalCols;
    const int totalCells;

    bool isPlaying{ true };
    float timeElapsed{ 0.0f };

};
