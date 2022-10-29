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

        LOGGER(info, "block O init succeeded");
    }

    ~BlockO() override = default;

    void init() override {
        generateBlock();
    }

};
