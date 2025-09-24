#include "Seripch.h"

#include "seri/core/Seri.h"
#include "seri/rendering/FramebufferBase.h"

#if defined (SERI_USE_RENDERING_OPENGL)
#include "seri/rendering/FramebufferOpenGL.h"
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
