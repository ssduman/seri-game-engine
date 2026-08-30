#pragma once

#include "seri/util/Util.h"
#include "seri/graphic/Graphic.h"
#include "seri/graphic/Material.h"
#include "seri/camera/CameraBase.h"
#include "seri/rendering/common/BufferBase.h"
#include "seri/rendering/common/FramebufferBase.h"
#include "seri/rendering/common/VertexArrayBase.h"
#include "seri/rendering/render/PipelineBase.h"
#include "seri/rendering/render/RenderingStats.h"

namespace seri
{
	class RenderCommandBufferBase
	{
	public:
		class RenderingManager;

		void Init();
		void Begin();
		void End();
		void Submit(RenderItem renderItem);
		void Submit(RenderCommand renderCommand);
		void SetState(RenderState state);
		void OnPassChanged(RenderPass renderPass);
		void Execute();

		virtual void Draw(DrawParams draw, const std::shared_ptr<VertexArrayBase>& vao) = 0;

		RenderingStats GetStats() { return _statsPrev; }

	protected:
		std::vector<RenderCommand> _commands{};

		RenderState _statePrev{};
		RenderingStats _stats{};
		RenderingStats _statsPrev{};
		FrameGraph _frameGraph{};

	};
}
