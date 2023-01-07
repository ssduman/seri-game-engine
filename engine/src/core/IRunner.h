#pragma once

#include "State.h"
#include "../util/Util.h"
#include "../scene/Layer.h"
#include "../logging/Logger.h"
#include "../window/IWindowManager.h"

class IRunner {
public:
    IRunner() = default;

    virtual ~IRunner() = default;
};
