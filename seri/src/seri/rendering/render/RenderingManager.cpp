#include "Seripch.h"

#include "seri/core/Seri.h"
#include "seri/rendering/render/RenderingManager.h"
#include "seri/rendering/common/RenderingManagerBase.h"

#ifdef __linux__
static_assert(false, "linux platform not supported");
#elif _WIN32

#if defined (SERI_USE_RENDERING_OPENGL)

#include "seri/rendering/opengl/RenderingManagerOpenGL.h"
#include "seri/rendering/opengl/RenderCommandBufferOpenGL.h"
std::unique_ptr<seri::RenderingManagerBase> seri::RenderingManager::_renderingManager = std::make_unique<seri::RenderingManagerOpenGL>();
std::unique_ptr<seri::RenderCommandBufferBase> seri::RenderingManager::_renderCommandBuffer = std::make_unique<seri::RenderCommandBufferOpenGL>();

#else

static_assert(false, "unknown rendering type");

#endif

#else
static_assert(false, "unknown platform");
#endif
