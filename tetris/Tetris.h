#pragma once

#include "../engine/Logger.h"
#include "../engine/Entity.h"

#include "Board.h"
#include "Camera.h"
#include "IBlock.h"
#include "BlockType.h"
#include "BlockFactory.h"
#include "TetrisMovement.h"
#include "TetrisProperties.h"

#include <vector>

class Tetris : public Entity {
public:
    Tetris(Camera* camera, TetrisProperties& tetrisProperties)
        : Entity(camera), _camera(camera), _tetrisProperties(tetrisProperties), _board(camera, _tetrisProperties) {
        LOGGER(info, "tetris init succeeded");
    }

    ~Tetris() override {
        for (auto& block : _blocks) {
            //delete block;
        }

        LOGGER(info, "tetris delete succeeded");
    }

    void init() override {
        _board.init();
        _currentBlock = BlockFactory::create(_camera, _tetrisProperties, BlockType::I);
        _blocks.emplace_back(_currentBlock);
    }

    void update() override {}

    void render() override {}

    void display() override {
        Object::display();

        for (auto& block : _blocks) {
            block->display();
        }

        _board.display();
    }

    void update(float deltaTime) {
        _tetrisProperties.timeElapsed += deltaTime;

        if (_tetrisProperties.timeElapsed > (0.016f * _tetrisProperties.speed)) {
            _tetrisProperties.timeElapsed = 0.0f;

            if (_currentBlock) {
                _currentBlock->move(_requestedBlockMovement);
            }
        }
    }

    void setRequestedBlockMovement(BlockMovement requestedBlockMovement) {
        _requestedBlockMovement = requestedBlockMovement;
    }

private:
    Camera* _camera{ nullptr };
    TetrisProperties& _tetrisProperties;
    Board _board;

    BlockMovement _requestedBlockMovement = BlockMovement::noop;
    IBlock* _currentBlock = nullptr;
    std::vector<IBlock*> _blocks;

};
