#include "Seripch.h"

#include "seri/core/Seri.h"
#include "seri/rendering/RenderingManagerFactory.h"

#ifdef __linux__
static_assert(false, "linux platform not supported");
#elif _WIN32

#if defined (SERI_USE_RENDERING_OPENGL)

#include "seri/rendering/RenderingManagerOpenGL.h"
std::unique_ptr<seri::RenderingManagerBase> seri::RenderingManagerFactory::_renderingManagerInstance = std::make_unique<seri::RenderingManagerOpenGL>();

#else

static_assert(false, "unknown rendering type");

#endif

#else
static_assert(false, "unknown platform");
#endif
