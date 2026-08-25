#include "Seripch.h"

#include "core/Seri.h"
#include "rendering/common/VertexArrayBase.h"

#if defined (SERI_USE_RENDERING_OPENGL)
#include "rendering/opengl/VertexArrayOpenGL.h"
#endif

namespace seri
{
	std::shared_ptr<VertexArrayBase> VertexArrayBase::Create()
	{
#if defined (SERI_USE_RENDERING_OPENGL)
		return std::make_shared<seri::VertexArrayOpenGL>();
#else
		static_assert(false, "unknown rendering type for creating vertex array");
#endif

		return nullptr;
	}
}
