#pragma once

class ICommand {
public:
    ICommand() = default;

    virtual ~ICommand() = default;

    virtual void execute() = 0;

};
