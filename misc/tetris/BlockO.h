#pragma once

#include "IBlock.h"

class BlockO : public IBlock {
public:
    BlockO(Camera* camera, TetrisProperties& tetrisProperties) : IBlock(camera, tetrisProperties) {
        _blockIdentifier = "O";
        _layout = {
            { 0, 0, 0, 0 },
            { 0, 1, 1, 0 },
            { 0, 1, 1, 0 },
            { 0, 0, 0, 0 },
        };
        _blockColor = { 0.95f, 0.95f, 0.0f, 1.0f };

        LOGGER(info, "block O init succeeded");
    }

    ~BlockO() override = default;

    void init() override {
        generateBlock();
    }

};
