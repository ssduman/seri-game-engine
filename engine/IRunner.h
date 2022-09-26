#pragma once

#include "Util.h"
#include "Layer.h"
#include "State.h"
#include "Typer.h"
#include "Light.h"
#include "Logger.h"
#include "Skybox.h"
#include "WindowManager.h"

class IRunner {
public:
    IRunner() = default;

    virtual ~IRunner() = default;
};
