#pragma once

#include "IBlock.h"

class BlockJ : public IBlock {
public:
    BlockJ(Camera* camera, TetrisProperties& tetrisProperties) : IBlock(camera, tetrisProperties) {
        _blockIdentifier = "J";
        _layout = {
            { 0, 0, 1, 0 },
            { 0, 0, 1, 0 },
            { 0, 1, 1, 0 },
            { 0, 0, 0, 0 },
        };
        _blockColor = { 0.0f, 0.0f, 0.94f, 1.0f };

        LOGGER(info, "block J init succeeded");
    }

    ~BlockJ() override = default;

    void init() override {
        generateBlock();
    }

};
