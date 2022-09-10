#pragma once

#include "ICommand.h"

class JumpCommand : public ICommand {
public:
    JumpCommand(Entity* entity, float jumpUnit) : _entity(entity), _jumpUnit(jumpUnit) {}
    
    ~JumpCommand() override = default;

    void execute() override {}

private:
    Entity* _entity;
    float _jumpUnit;

};
