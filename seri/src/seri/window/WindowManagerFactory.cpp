#include "Seripch.h"

#include "seri/core/Seri.h"
#include "seri/window/WindowManagerFactory.h"

#ifdef __linux__
static_assert(false, "linux platform not supported");
#elif _WIN32

#if defined (SERI_USE_GLFW)

#include "seri/window/WindowsGLFWWindowManager.h"
std::unique_ptr<seri::IWindowManager> seri::WindowManagerFactory::_windowManagerInstance = std::make_unique<seri::WindowsGLFWWindowManager>();

#elif defined (SERI_USE_SDL3)

#include "seri/window/WindowsSDLWindowManager.h"
std::unique_ptr<seri::IWindowManager> seri::WindowManagerFactory::_windowManagerInstance = std::make_unique<seri::WindowsSDLWindowManager>();

#else

static_assert(false, "unknown window type");

#endif

#else
static_assert(false, "unknown platform");
#endif
