#pragma once

#include "Camera.h"
#include "IBlock.h"
#include "BlockI.h"
#include "BlockJ.h"
#include "BlockL.h"
#include "BlockO.h"
#include "BlockS.h"
#include "BlockT.h"
#include "BlockType.h"
#include "BlockZ.h"
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
            case BlockType::L:
            {
                auto block = new BlockL{ camera, tetrisProperties };
                block->init();
                return block;
            }
            case BlockType::O:
            {
                auto block = new BlockO{ camera, tetrisProperties };
                block->init();
                return block;
            }
            case BlockType::S:
            {
                auto block = new BlockS{ camera, tetrisProperties };
                block->init();
                return block;
            }
            case BlockType::T:
            {
                auto block = new BlockT{ camera, tetrisProperties };
                block->init();
                return block;
            }
            case BlockType::Z:
            {
                auto block = new BlockZ{ camera, tetrisProperties };
                block->init();
                return block;
            }
            default:;
        }

        return nullptr;
    }

};
