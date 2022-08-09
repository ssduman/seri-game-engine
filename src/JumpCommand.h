#pragma once

#include "ICommand.h"

class JumpCommand : public ICommand {
public:
    JumpCommand(Entity* entity, float jumpUnit) : _entity(entity), _jumpUnit(jumpUnit) {};
    
    virtual ~JumpCommand() {}

    void execute() override {}

private:
    Entity* _entity;
    float _jumpUnit;

};
