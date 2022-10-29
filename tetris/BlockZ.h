#pragma once

#include "IBlock.h"

class BlockZ : public IBlock {
public:
    BlockZ(Camera* camera, TetrisProperties& tetrisProperties) : IBlock(camera, tetrisProperties) {
        _blockIdentifier = "Z";
        _layout = {
            { 0, 0, 0, 0 },
            { 1, 1, 0, 0 },
            { 0, 1, 1, 0 },
            { 0, 0, 0, 0 },
        };
        _row = 3;

        LOGGER(info, "block Z init succeeded");
    }

    ~BlockZ() override = default;

    void init() override {
        generateBlock();
    }

};
