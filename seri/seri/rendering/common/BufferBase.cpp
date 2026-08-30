#include "Seripch.h"

#include "seri/core/Seri.h"
#include "seri/rendering/common/BufferBase.h"

#if defined (SERI_USE_RENDERING_OPENGL)
#include "seri/rendering/opengl/BufferOpenGL.h"
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

	std::shared_ptr<IndexBufferBase> IndexBufferBase::Create(const std::vector<uint32_t>& indices)
	{
		return IndexBufferBase::Create(indices.data(), indices.size());
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

	std::shared_ptr<VertexBufferBase> VertexBufferBase::Create(const std::vector<glm::vec2>& data, BufferUsage usage)
	{
		return VertexBufferBase::Create(data.data(), data.size() * sizeof(glm::vec2), usage);
	}

	std::shared_ptr<VertexBufferBase> VertexBufferBase::Create(const std::vector<glm::vec3>& data, BufferUsage usage)
	{
		return VertexBufferBase::Create(data.data(), data.size() * sizeof(glm::vec3), usage);
	}

	std::shared_ptr<VertexBufferBase> VertexBufferBase::Create(const std::vector<glm::vec4>& data, BufferUsage usage)
	{
		return VertexBufferBase::Create(data.data(), data.size() * sizeof(glm::vec4), usage);
	}

	std::shared_ptr<VertexBufferBase> VertexBufferBase::Create(const std::vector<glm::mat4>& data, BufferUsage usage)
	{
		return VertexBufferBase::Create(data.data(), data.size() * sizeof(glm::mat4), usage);
	}

	std::shared_ptr<UniformBufferBase> seri::UniformBufferBase::Create(uint32_t size, UniformBinding binding)
	{
#if defined (SERI_USE_RENDERING_OPENGL)
		return std::make_shared<seri::UniformBufferOpenGL>(size, binding);
#else
		static_assert(false, "unknown rendering type for creating uniform buffer");
#endif

		return nullptr;
	}

}
