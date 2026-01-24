#pragma once

#include "seri/util/Util.h"
#include "seri/graphic/Material.h"
#include "seri/camera/CameraBase.h"
#include "seri/rendering/common/BufferBase.h"
#include "seri/rendering/common/FramebufferBase.h"
#include "seri/rendering/common/VertexArrayBase.h"
#include "seri/rendering/common/RenderingManagerBase.h"
#include "seri/rendering/render/PipelineBase.h"
#include "seri/rendering/render/RenderingStats.h"

namespace seri
{
	class RenderCommandBufferBase
	{
	public:
		virtual void Init() = 0;
		virtual void Begin() = 0;
		virtual void End() = 0;
		virtual void Submit(RenderCommand renderCommand) = 0;
		virtual void Execute() = 0;

		RenderingStats GetStats()
		{
			return _statsPrev;
		}

	protected:
		std::vector<RenderCommand> _commands{};

		RenderingStats _stats{};
		RenderingStats _statsPrev{};
		FrameGraph _frameGraph{};

	};
}
