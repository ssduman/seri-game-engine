#pragma once

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
		size_t size;
		BufferUsage usage;
		BufferTarget target;
	};

	class BufferBase
	{
	public:
		virtual ~BufferBase() = default;

	};
}
