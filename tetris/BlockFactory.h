#pragma once

#include "Camera.h"
#include "IBlock.h"
#include "BlockI.h"
#include "BlockJ.h"
#include "BlockType.h"
#include "TetrisProperties.h"

class BlockFactory {
public:
    static IBlock* create(Camera* camera, TetrisProperties& tetrisProperties, BlockType blockType) {
        switch (blockType) {
            case BlockType::I:
            {
                auto block = new BlockI{ camera, tetrisProperties };
                block->init();
                return block;
            }
            case BlockType::J:
            {
                auto block = new BlockJ{ camera, tetrisProperties };
                block->init();
                return block;
            }
            case BlockType::L: break;
            case BlockType::O: break;
            case BlockType::S: break;
            case BlockType::T: break;
            case BlockType::Z: break;
            default:;
        }

        return nullptr;
    }

};
