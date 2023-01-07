#include "Seripch.h"

#include "WindowFactory.h"

#ifdef __linux__
static_assert(false, "linux platform not supported");
#elif _WIN32
std::shared_ptr<IWindowManager> WindowFactory::_windowManagerInstance = std::make_shared<WindowsWindowManager>();
#else
static_assert(false, "unknown platform");
#endif
