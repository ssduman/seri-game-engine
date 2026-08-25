#include "Seripch.h"

#include "core/Seri.h"
#include "shader/ShaderBase.h"

#if defined (SERI_USE_RENDERING_OPENGL)
#include "shader/ShaderOpenGL.h"
#endif

namespace seri
{
	std::shared_ptr<ShaderBase> ShaderBase::Create()
	{
#if defined (SERI_USE_RENDERING_OPENGL)
		return std::make_shared<seri::ShaderOpenGL>();
#else
		static_assert(false, "unknown rendering type for creating shader");
#endif

		return nullptr;
	}
}
