#pragma once

#include "IWindowManager.h"
#include "WindowsWindowManager.h"

#include <memory>

class WindowFactory {
public:
    static std::shared_ptr<IWindowManager> instance() {
        return _windowManagerInstance;
    }

private:
    static std::shared_ptr<IWindowManager> _windowManagerInstance;

};