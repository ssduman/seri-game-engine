#pragma once

#include "util/Util.h"
#include "graphic/Graphic.h"
#include "graphic/Material.h"
#include "camera/CameraBase.h"
#include "rendering/common/BufferBase.h"
#include "rendering/common/FramebufferBase.h"
#include "rendering/common/VertexArrayBase.h"
#include "rendering/render/PipelineBase.h"
#include "rendering/render/RenderingStats.h"

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
