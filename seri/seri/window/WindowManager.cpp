#include "Seripch.h"

#include "seri/core/Seri.h"
#include "seri/window/WindowManager.h"

#if defined (SERI_USE_WINDOW_GLFW)

#include "seri/window/WindowManagerGLFW.h"
std::unique_ptr<seri::WindowManagerBase> seri::WindowManager::_windowManager = std::make_unique<seri::WindowManagerGLFW>();

#elif defined (SERI_USE_WINDOW_SDL3)

#include "seri/window/WindowManagerSDL.h"
std::unique_ptr<seri::WindowManagerBase> seri::WindowManager::_windowManager = std::make_unique<seri::WindowManagerSDL>();

#else

static_assert(false, "unknown window type");

#endif
