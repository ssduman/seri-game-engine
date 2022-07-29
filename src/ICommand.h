#pragma once

#include "Entity.h"

class ICommand {
public:
    ICommand() = default;
    ~ICommand() = default;

    virtual void execute() = 0;

private:

};
