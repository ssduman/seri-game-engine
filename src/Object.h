#pragma once

#include <iostream>

class Object {
public:
    virtual ~Object() {}

    virtual void init() = 0;

    virtual void update() = 0;

    virtual void render() = 0;

private:

};
