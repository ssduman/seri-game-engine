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
        _blockColor = { 0.95f, 0.0f, 0.0f, 1.0f };

        LOGGER(info, "block Z init succeeded");
    }

    ~BlockZ() override = default;

    void init() override {
        generateBlock();
    }

};
