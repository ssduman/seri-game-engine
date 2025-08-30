#include "Seripch.h"

#include "seri/core/Seri.h"
#include "seri/window/WindowManagerFactory.h"

#ifdef __linux__
static_assert(false, "linux platform not supported");
#elif _WIN32

#if defined (SERI_USE_WINDOW_GLFW)

#include "seri/window/WindowManagerGLFW.h"
std::unique_ptr<seri::WindowManagerBase> seri::WindowManagerFactory::_windowManagerInstance = std::make_unique<seri::WindowManagerGLFW>();

#elif defined (SERI_USE_WINDOW_SDL3)

#include "seri/window/WindowManagerSDL.h"
std::unique_ptr<seri::WindowManagerBase> seri::WindowManagerFactory::_windowManagerInstance = std::make_unique<seri::WindowManagerSDL>();

#else

static_assert(false, "unknown window type");

#endif

#else
static_assert(false, "unknown platform");
#endif
