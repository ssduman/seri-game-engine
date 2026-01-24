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
		void Init() override;
		void Begin() override;
		void End() override;
		void Submit(RenderCommand renderCommand) override;
		void Execute() override;

	private:
		GLenum GetTopology(Topology topology);
		GLenum GetDataType(DataType dataType);

	};
}
