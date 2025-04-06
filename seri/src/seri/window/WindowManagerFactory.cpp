#include "Seripch.h"

#include "seri/window/WindowManagerFactory.h"
#include "seri/window/WindowsWindowManager.h"

#ifdef __linux__
static_assert(false, "linux platform not supported");
#elif _WIN32
std::shared_ptr<seri::IWindowManager> seri::WindowManagerFactory::_windowManagerInstance = std::make_shared<seri::WindowsWindowManager>();
#else
static_assert(false, "unknown platform");
#endif
