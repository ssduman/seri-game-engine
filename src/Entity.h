#pragma once

#include "Object.h"

#include <string>

class Entity : public Object {
public:
    virtual void registerShader(const std::string& vs_path, const std::string& fs_path) = 0;

private:

};
