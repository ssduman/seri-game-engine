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
        _blockColor = { 0.63f, 0.0f, 0.94f, 1.0f };

        LOGGER(info, "block T init succeeded");
    }

    ~BlockT() override = default;

    void init() override {
        generateBlock();
    }

};
