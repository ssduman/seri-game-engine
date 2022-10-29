#pragma once

#include "IBlock.h"

class BlockT : public IBlock {
public:
    BlockT(Camera* camera, TetrisProperties& tetrisProperties) : IBlock(camera, tetrisProperties) {
        _blockIdentifier = "T";
        _layout = {
            { 0, 0, 0, 0 },
            { 1, 1, 1, 0 },
            { 0, 1, 0, 0 },
            { 0, 0, 0, 0 },
        };
        _row = 3;

        LOGGER(info, "block T init succeeded");
    }

    ~BlockT() override = default;

    void init() override {
        generateBlock();
    }

};
