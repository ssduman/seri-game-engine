#include "Seripch.h"

#include "seri/core/Seri.h"
#include "seri/renderer/RenderingManagerFactory.h"

#ifdef __linux__
static_assert(false, "linux platform not supported");
#elif _WIN32

#if defined (SERI_USE_OPENGL)

#include "seri/renderer/OpenGLRenderingManager.h"
std::unique_ptr<seri::IRenderingManager> seri::RenderingManagerFactory::_renderingManagerInstance = std::make_unique<seri::OpenGLRenderingManager>();

#elif defined (SERI_USE_D3D11)

static_assert(false, "d3d11 not supported");

#else

static_assert(false, "unknown rendering type");

#endif

#else
static_assert(false, "unknown platform");
#endif
