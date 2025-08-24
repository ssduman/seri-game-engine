#include "Seripch.h"

#include "seri/core/Seri.h"
#include "seri/renderer/RenderingManagerFactory.h"

#ifdef __linux__
static_assert(false, "linux platform not supported");
#elif _WIN32

#if defined (SERI_USE_RENDERING_OPENGL)

#include "seri/renderer/OpenGLRenderingManager.h"
std::unique_ptr<seri::IRenderingManager> seri::RenderingManagerFactory::_renderingManagerInstance = std::make_unique<seri::OpenGLRenderingManager>();

#else

static_assert(false, "unknown rendering type");

#endif

#else
static_assert(false, "unknown platform");
#endif
