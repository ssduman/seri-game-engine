#pragma once

#include "IBlock.h"

class BlockS : public IBlock {
public:
    BlockS(Camera* camera, TetrisProperties& tetrisProperties) : IBlock(camera, tetrisProperties) {
        _blockIdentifier = "S";
        _layout = {
            { 0, 0, 0, 0 },
            { 0, 1, 1, 0 },
            { 1, 1, 0, 0 },
            { 0, 0, 0, 0 },
        };

        LOGGER(info, "block S init succeeded");
    }

    ~BlockS() override = default;

    void init() override {
        generateBlock();
    }

};
