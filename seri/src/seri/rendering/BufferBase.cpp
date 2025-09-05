#include "Seripch.h"

#include "seri/core/Seri.h"
#include "seri/rendering/BufferBase.h"

#if defined (SERI_USE_RENDERING_OPENGL)
#include "seri/rendering/BufferOpenGL.h"
#endif

namespace seri
{
	std::shared_ptr<IndexBufferBase> IndexBufferBase::Create(const uint32_t* data, uint32_t count)
	{
#if defined (SERI_USE_RENDERING_OPENGL)
		return std::make_shared<seri::IndexBufferOpenGL>(data, count);
#else
		static_assert(false, "unknown rendering type for creating index buffer");
#endif

		return nullptr;
	}

	std::shared_ptr<VertexBufferBase> VertexBufferBase::Create(const void* data, uint32_t size, BufferUsage usage)
	{
#if defined (SERI_USE_RENDERING_OPENGL)
		return std::make_shared<seri::VertexBufferOpenGL>(data, size, usage);
#else
		static_assert(false, "unknown rendering type for creating vertex buffer");
#endif

		return nullptr;
	}
}
