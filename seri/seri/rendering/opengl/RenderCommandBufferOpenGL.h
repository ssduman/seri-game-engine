#pragma once

#include "seri/graphic/Graphic.h"
#include "seri/rendering/render/RenderingManager.h"
#include "seri/rendering/common/RenderCommandBufferBase.h"

#include <glad/gl.h>

namespace seri
{
	class RenderCommandBufferOpenGL : public RenderCommandBufferBase
	{
	public:
		void Draw(DrawParams draw, const std::shared_ptr<VertexArrayBase>& vao) override;

	private:
		GLenum GetTopology(Topology topology);
		GLenum GetDataType(DataType dataType);

	};
}
