#pragma once

#include "seri/util/Util.h"
#include "seri/rendering/BufferBase.h"

namespace seri
{
	class VertexArrayBase
	{
	public:
		virtual ~VertexArrayBase() = default;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual const std::shared_ptr<IndexBufferBase>& GetIndexBuffer() = 0;
		virtual void SetIndexBuffer(const std::shared_ptr<IndexBufferBase>& indexBuffer) = 0;
		virtual void AddVertexBuffer(const std::shared_ptr<VertexBufferBase>& vertexBuffer) = 0;

		static std::shared_ptr<VertexArrayBase> Create();

	protected:
		std::shared_ptr<IndexBufferBase> _indexBuffer;
		std::vector<std::shared_ptr<VertexBufferBase>> _vertexBuffers;

	};
}
