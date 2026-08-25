#include "Seripch.h"

#include "core/Seri.h"
#include "rendering/common/FramebufferBase.h"

#if defined (SERI_USE_RENDERING_OPENGL)
#include "rendering/opengl/FramebufferOpenGL.h"
#endif

namespace seri
{
	std::shared_ptr<FramebufferBase> FramebufferBase::Create()
	{
#if defined (SERI_USE_RENDERING_OPENGL)
		return std::make_shared<seri::FramebufferOpenGL>();
#else
		static_assert(false, "unknown rendering type for creating framebuffer");
#endif

		return nullptr;
	}

	std::shared_ptr<FramebufferBase> FramebufferBase::Create(FramebufferDesc desc)
	{
#if defined (SERI_USE_RENDERING_OPENGL)
		return std::make_shared<seri::FramebufferOpenGL>(desc);
#else
		static_assert(false, "unknown rendering type for creating framebuffer");
#endif

		return nullptr;
	}
}
