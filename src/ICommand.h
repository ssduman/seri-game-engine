#pragma once

#include "Entity.h"
#include "Camera.h"

class ICommand {
public:
    ICommand() = default;
    
    virtual ~ICommand() = 0;

    virtual void execute() = 0;

};
