#pragma once

#include "IBlock.h"

class BlockL : public IBlock {
public:
    BlockL(Camera* camera, TetrisProperties& tetrisProperties) : IBlock(camera, tetrisProperties) {
        _blockIdentifier = "L";
        _layout = {
            { 0, 1, 0, 0 },
            { 0, 1, 0, 0 },
            { 0, 1, 1, 0 },
            { 0, 0, 0, 0 },
        };
        _row = 3;

        LOGGER(info, "block L init succeeded");
    }

    ~BlockL() override = default;

    void init() override {
        generateBlock();
    }

};
