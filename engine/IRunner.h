#pragma once

#include "Util.h"
#include "Layer.h"
#include "State.h"
#include "Logger.h"
#include "WindowManager.h"

class IRunner {
public:
    IRunner() = default;

    virtual ~IRunner() = default;
};
