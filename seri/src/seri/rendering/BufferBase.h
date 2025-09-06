#pragma once

#include "seri/util/Util.h"
#include "seri/rendering/RenderingUtil.h"

namespace seri
{
	enum class BufferTarget
	{
		vertex,
		index,
	};

	enum class BufferUsage
	{
		stream_draw,
		static_draw,
		dynamic_draw,
	};

	struct BufferDesc
	{
		BufferUsage usage;
		BufferTarget target;
	};

	class IndexBufferBase
	{
	public:
		virtual ~IndexBufferBase() = default;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual uint32_t GetCount() = 0;

		static std::shared_ptr<IndexBufferBase> Create(const uint32_t* data, uint32_t count);

	protected:
		uint32_t _count;

	};

	class VertexBufferBase
	{
	public:
		virtual ~VertexBufferBase() = default;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) = 0;

		virtual const BufferLayoutDesc& GetLayout() = 0;
		virtual void SetLayout(const BufferLayoutDesc& layout) = 0;
		virtual VertexBufferBase& AddElement(const BufferElementDesc& element) = 0;

		static std::shared_ptr<VertexBufferBase> Create(const void* data, uint32_t size, BufferUsage usage = BufferUsage::static_draw);

	protected:
		BufferLayoutDesc _layoutDesc{};

	};
}
