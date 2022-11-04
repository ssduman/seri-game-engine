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
        _blockColor = { 0.94f, 0.63f, 0.0f, 1.0f };

        LOGGER(info, "block L init succeeded");
    }

    ~BlockL() override = default;

    void init() override {
        generateBlock();
    }

};
