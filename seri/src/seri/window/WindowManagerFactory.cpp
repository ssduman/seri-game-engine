#include "Seripch.h"

#include <seri/core/Seri.h>
#include "seri/window/WindowManagerFactory.h"
#include "seri/window/WindowsGLFWWindowManager.h"
#include "seri/window/WindowsSDLWindowManager.h"

#ifdef __linux__
static_assert(false, "linux platform not supported");
#elif _WIN32

#ifdef SERI_USE_GLFW
std::unique_ptr<seri::IWindowManager> seri::WindowManagerFactory::_windowManagerInstance = std::make_unique<seri::WindowsGLFWWindowManager>();
#elif defined (SERI_USE_SDL3)
std::unique_ptr<seri::IWindowManager> seri::WindowManagerFactory::_windowManagerInstance = std::make_unique<seri::WindowsSDLWindowManager>();
#else
static_assert(false, "unknown window type");
#endif

#else
static_assert(false, "unknown platform");
#endif
