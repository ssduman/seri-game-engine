#pragma once

#if defined _WIN32

#define SERI_WINDOWS
#define WIN32_LEAN_AND_MEAN

#elif defined linux

#define SERI_LINUX

static_assert(false, "linux not supported");

#endif

#define SERI_MULTITHREADED 0

#define SERI_USE_WINDOW_GLFW
//#define SERI_USE_WINDOW_SDL3

#define SERI_USE_RENDERING_OPENGL
//#define SERI_USE_RENDERING_D3D11
//#define SERI_USE_RENDERING_D3D12
//#define SERI_USE_RENDERING_VULKAN
//#define SERI_USE_RENDERING_NULL

#if defined SERI_USE_WINDOW_SDL3
#define SDL_MAIN_HANDLED
#define SDL_MAIN_USE_CALLBACKS 1
#endif
