#pragma once

#include "IBlock.h"

class BlockI : public IBlock {
public:
    BlockI(Camera* camera, TetrisProperties& tetrisProperties) : IBlock(camera, tetrisProperties) {
        _blockIdentifier = "I";
        _layout = {
            { 0, 0, 0, 0 },
            { 1, 1, 1, 1 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
        };

        LOGGER(info, "block I init succeeded");
    }

    ~BlockI() override = default;

    void init() override {
        generateBlock();
    }

};
